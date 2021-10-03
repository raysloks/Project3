import bpy
import struct


class DummyAnimationChannel:
    def __init__(self):
        self.keyframe_points = []


def write_anm(self, context, filepath):
    print("running write_anm...")
    f = open(filepath, 'wb')
    
    #context = bpy.context
    scene = context.scene
    
    abs_filepath = bpy.path.abspath(filepath)
    
    default_animation_channels = [\
        ['location', 0],\
        ['location', 1],\
        ['location', 2],\
        ['rotation_quaternion', 0],\
        ['rotation_quaternion', 1],\
        ['rotation_quaternion', 2],\
        ['rotation_quaternion', 3],\
        ['scale', 0],\
        ['scale', 1],\
        ['scale', 2]\
    ]

    for ob in scene.objects:
        if not ob.users_collection[0].hide_render and ob.type == 'MESH':
            armature = ob.find_armature().data
            flags = '00000001'
            f.write(struct.pack('<B', int(flags, 2)))
            f.write(struct.pack('<B', len(armature.bones)))
            f.write(struct.pack('<6B', 0, 0, 0, 0, 0, 0))
            for bone in armature.bones:
                matrix = bone.matrix_local
                if bone.parent:
                    matrix = bone.parent.matrix_local.inverted() @ matrix
                for row in matrix.transposed():
                    for value in row:
                        f.write(struct.pack('<f', value))
                if bone.parent:
                    f.write(struct.pack('<Q', armature.bones.find(bone.parent.name)))
                else:
                    f.write(struct.pack('<Q', 255))
            f.write(struct.pack('<Q', len(bpy.data.actions)))
            for action in bpy.data.actions:
                if action.use_fake_user:
                    f.write(struct.pack('<Q', len(action.name)))
                    for char in action.name:
                        f.write(struct.pack('<B', ord(char)))
                    for bone in armature.bones:
                        if bone.name in action.groups:
                            group = action.groups[bone.name]
                            channels = [next((channel for channel in group.channels if channel.data_path.endswith(x[0]) and channel.array_index == x[1]), DummyAnimationChannel()) for x in default_animation_channels]
                            for channel in channels:
                                f.write(struct.pack('<Q', len(channel.keyframe_points)))
                                for keyframe_point in channel.keyframe_points:
                                    f.write(struct.pack('<2f', keyframe_point.co[0], keyframe_point.co[1]))
                        else:
                            for channel in default_animation_channels:
                                f.write(struct.pack('<Q', 0))
            break
    
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty


class ExportANM(bpy.types.Operator, ExportHelper):
    '''Exports a .anm file.'''
    bl_idname = "export.anm"
    bl_label = "Export ANM"

    # ExportHelper mixin class uses this
    filename_ext = ".anm"

    filter_glob : StringProperty(default="*.anm", options={'HIDDEN'})

    @classmethod
    def poll(cls, context):
        return context.active_object != None

    def execute(self, context):
        return write_anm(self, context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportANM.bl_idname, text="Animation (.anm)")


def register():
    bpy.utils.register_class(ExportANM)
    #bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportANM)
    #bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export.anm('INVOKE_DEFAULT')
