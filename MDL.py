import bpy
import struct

def write_mdl(self, context, filepath):
    print("running write_mdl...")
    f = open(filepath, 'wb')
    
    #context = bpy.context
    scene = context.scene
    
    abs_filepath = bpy.path.abspath(filepath)

    for ob in bpy.context.scene.objects:
        if not ob.users_collection[0].hide_render and ob.type == 'MESH':
            mesh = ob.evaluated_get(context.evaluated_depsgraph_get()).to_mesh()
            f.write(struct.pack('<I', len(mesh.loops)))
            for loop in mesh.loops:
                co = mesh.vertices[loop.vertex_index].co
                f.write(struct.pack('<3f', co[0], co[1], co[2]))
                uv = mesh.uv_layers[0].data[loop.index].uv
                f.write(struct.pack('<2f', uv[0], 1.0 - uv[1]))
            f.write(struct.pack('<I', len(mesh.polygons)))
            for triangle in mesh.polygons:
                idx = triangle.loop_indices
                f.write(struct.pack('<3H', idx[0], idx[1], idx[2]))
            break
    
    f.close()

    return {'FINISHED'}

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty


class ExportMDL(bpy.types.Operator, ExportHelper):
    '''Exports a .mdl file.'''
    bl_idname = "export.mdl"
    bl_label = "Export MDL"

    # ExportHelper mixin class uses this
    filename_ext = ".mdl"

    filter_glob : StringProperty(default="*.mdl", options={'HIDDEN'})

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