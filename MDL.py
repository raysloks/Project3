import bpy
import struct


class DummyVertexGroup:
    def __init__(self, group, weight):
        self.group = group
        self.weight = weight


def write_mdl(self, context, filepath):
    print("running write_mdl...")
    f = None
    if not self.split_objects:
        f = open(filepath, 'wb')
    
    #context = bpy.context
    scene = context.scene
    
    abs_filepath = bpy.path.abspath(filepath)

    for ob in context.collection.objects:
        if not ob.users_collection[0].hide_render and ob.type == 'MESH':
            
            if self.split_objects:
                f = open(filepath[:-3] + ob.name + '.mdl', 'wb')
            
            depsgraph = context.evaluated_depsgraph_get()
            ob = ob.evaluated_get(depsgraph)
            mesh = ob.to_mesh(preserve_all_data_layers=True, depsgraph=depsgraph)
            armature = ob.find_armature()
            flags = list('00000011')
            #if armature == None:
                #flags[6] = '0'
            f.write(struct.pack('<B', int("".join(flags), 2)))
            f.write(struct.pack('<H', len(mesh.loops)))
            for loop in mesh.loops:
                vertex = mesh.vertices[loop.vertex_index]
                
                co = vertex.co
                f.write(struct.pack('<3f', co[0], co[1], co[2]))
                
                uv = [0, 0]
                if len(mesh.uv_layers) > 0:
                    mesh.uv_layers[0].data[loop.index].uv
                f.write(struct.pack('<2f', uv[0], 1.0 - uv[1]))
                
                normal = vertex.normal
                f.write(struct.pack('<3f', normal[0], normal[1], normal[2]))
                
                if armature != None:
                    bones = ob.find_armature().pose.bones
                    groups = filter(lambda x: x.weight > 0.0, vertex.groups)
                    groups = sorted(groups, key=lambda x: x.weight, reverse=True)
                    if len(groups) > 0:
                        groups = groups + [DummyVertexGroup(groups[0].group, 0.0)] * 3
                    else:
                        groups = [DummyVertexGroup(0, 1.0)] + [DummyVertexGroup(0, 0.0)] * 3
                    groups = groups[:4]
                    total_weight = sum(x.weight for x in groups) or 1
                    for group in groups:
                        f.write(struct.pack('<B', bones.find(ob.vertex_groups[group.group].name)))
                    for group in groups:
                        f.write(struct.pack('<f', group.weight / total_weight))
                else:
                    f.write(struct.pack('<4B', 0, 0, 0, 0))
                    f.write(struct.pack('<4f', 1.0, 0.0, 0.0, 0.0))
                    
            f.write(struct.pack('<H', len(mesh.polygons)))
            for triangle in mesh.polygons:
                idx = triangle.loop_indices
                f.write(struct.pack('<3H', idx[0], idx[1], idx[2]))
            if not self.split_objects:
                break
            else:
                f.close()
    
    if not self.split_objects:
        f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty
from bpy.props import BoolProperty


class ExportMDL(bpy.types.Operator, ExportHelper):
    '''Exports a .mdl file.'''
    bl_idname = "export.mdl"
    bl_label = "Export MDL"

    # ExportHelper mixin class uses this
    filename_ext = ".mdl"

    filter_glob : StringProperty(default="*.mdl", options={'HIDDEN'})
    
    split_objects : BoolProperty(name="Split Objects", default=False)

    @classmethod
    def poll(cls, context):
        return context.active_object != None

    def execute(self, context):
        return write_mdl(self, context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportMDL.bl_idname, text="Model (.mdl)")


def register():
    bpy.utils.register_class(ExportMDL)
    #bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportMDL)
    #bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export.mdl('INVOKE_DEFAULT')
