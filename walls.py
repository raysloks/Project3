bl_info = {
    "name": "New Object",
    "author": "Your Name Here",
    "version": (1, 0),
    "blender": (2, 80, 0),
    "location": "View3D > Add > Mesh > New Object",
    "description": "Adds a new Mesh Object",
    "warning": "",
    "doc_url": "",
    "category": "Add Mesh",
}


import bpy
from bpy.types import Operator
from bpy.props import FloatVectorProperty
from bpy_extras.object_utils import AddObjectHelper, object_data_add
from mathutils import Vector
from math import radians
from operator import add


def rotate_mask_left(mask, steps):
    return ((mask << steps) | (mask >> (8 - steps))) & 255

def rotate_mask_right(mask, steps):
    return ((mask >> steps) | (mask << (8 - steps))) & 255

def rotate_index_left(index, steps):
    return (index + steps) % 8

def rotate_index_right(index, steps):
    return (index - steps) % 8

def translate_mask_basic(mask, index):
    tables = [
        [0, 0, 1, 1, -1, 7, 7, 0],
        [0, 1, 2, 2, 3, -1, 7, 0]
    ]
    table = tables[index]
    new_mask = 0
    for i in range(8):
        target_index = table[i]
        if target_index != -1:
            if mask & 1 << target_index != 0:
                new_mask |= 1 << i
    return new_mask

def translate_mask(mask, index):
    rotation = (index // 2) * 2
    mask = rotate_mask_right(mask, rotation)
    index = rotate_index_right(index, rotation)
    return rotate_mask_left(translate_mask_basic(mask, index), rotation)

def translate_mask_wall(mask, index):
    return translate_mask(mask, index) | rotate_mask_left(1 << index, 4)

def translate_mask_air(mask, index):
    return translate_mask(mask, index) & rotate_mask_left(~(1 << index), 4)

def create_wall(context, collection, mask, location):
    
    air_masks = [
        '10101010',
        '10101000',
        '00101000',
        '00001000',
        '00010000'
    ]
    wall_masks = [
        '00000000',
        '00000010',
        '10000010',
        '00100010',
        '00101000'
    ]
    
    air_masks = [int(x, 2) for x in air_masks]
    wall_masks = [int(x, 2) for x in wall_masks]
    
    templates = [bpy.data.objects['wall' + str(i)] for i in range(5)]
    floor_template = bpy.data.objects['floor']
    
    wall_objects = []
    for rotation_step in range(4):
        rotated_mask = rotate_mask_left(mask, rotation_step * 2)
        for i in range(5):
            if rotation_step > 0 and i == 0:
                continue
            if rotated_mask & air_masks[i] != 0:
                continue
            if ~rotated_mask & wall_masks[i] != 0:
                continue
            src_obj = templates[i]
            new_obj = src_obj.copy()
            new_obj.location = location
            new_obj.rotation_euler = [0.0, 0.0, rotation_step * radians(90.0)]
            new_obj.data = src_obj.data.copy()
            
            collection.objects.link(new_obj)
            wall_objects.append(new_obj)
    src_obj = floor_template 
    new_obj = src_obj.copy()
    new_obj.location = location
    new_obj.data = src_obj.data.copy()
    
    collection.objects.link(new_obj)
    wall_objects.append(new_obj)
    
    ctx = context.copy()

    ctx['active_object'] = wall_objects[0]
    ctx['selected_editable_objects'] = wall_objects

    bpy.ops.object.join(ctx)
    
    return ctx['active_object']


def add_object(self, context):

    offsets = [
        [-1, 1, 0],
        [0, 1, 0],
        [1, 1, 0],
        [1, 0, 0],
        [1, -1, 0],
        [0, -1, 0],
        [-1, -1, 0],
        [-1, 0, 0]
    ]
    
    main_mask_index = 0
    
    occluders = bpy.data.collections.new('Occluders')
    bpy.context.scene.collection.children.link(occluders)
    
    prev_masks = {}
    for main_mask in range(256):
        if main_mask in prev_masks:
            continue
        for rotation_step in range(4):
            mask = rotate_mask_left(main_mask, rotation_step * 2)
            prev_masks[mask] = main_mask
        
        location = [(main_mask_index % 16) * 10, (main_mask_index // 16) * 10 + 10, 0]
        
        main_wall_object = create_wall(context, context.collection, main_mask, location)
        main_wall_object.name = 'wall_' + str(main_mask)
        
        for i in range(8):
            if main_mask & 1 << i != 0:
                create_wall(context, occluders, translate_mask_wall(main_mask, i), list(map(add, location, offsets[i])))
            else:
                create_wall(context, occluders, 255, list(map(add, location, offsets[i])))
        
        main_mask_index += 1
    
    prev_masks = {}
    for main_mask in range(256):
        if main_mask in prev_masks:
            continue
        for rotation_step in range(4):
            mask = rotate_mask_left(main_mask, rotation_step * 2)
            prev_masks[mask] = main_mask
        
        location = [(main_mask_index % 16) * 10, (main_mask_index // 16) * 10 + 10, 0]
        
        main_wall_object = create_wall(context, context.collection, 255, location)
        main_wall_object.name = 'floor_' + str(main_mask)
        
        for i in range(8):
            if main_mask & 1 << i != 0:
                create_wall(context, occluders, translate_mask_air(main_mask, i), list(map(add, location, offsets[i])))
            else:
                create_wall(context, occluders, 255, list(map(add, location, offsets[i])))
        
        main_mask_index += 1


class OBJECT_OT_add_object(Operator, AddObjectHelper):
    """Create a new Mesh Object"""
    bl_idname = "mesh.add_object"
    bl_label = "Add Mesh Object"
    bl_options = {'REGISTER', 'UNDO'}

    scale: FloatVectorProperty(
        name="scale",
        default=(1.0, 1.0, 1.0),
        subtype='TRANSLATION',
        description="scaling",
    )

    def execute(self, context):

        add_object(self, context)

        return {'FINISHED'}


# Registration

def add_object_button(self, context):
    self.layout.operator(
        OBJECT_OT_add_object.bl_idname,
        text="Add Object",
        icon='PLUGIN')


# This allows you to right click on a button and link to documentation
def add_object_manual_map():
    url_manual_prefix = "https://docs.blender.org/manual/en/latest/"
    url_manual_mapping = (
        ("bpy.ops.mesh.add_object", "scene_layout/object/types.html"),
    )
    return url_manual_prefix, url_manual_mapping


def register():
    bpy.utils.register_class(OBJECT_OT_add_object)
    bpy.utils.register_manual_map(add_object_manual_map)
    bpy.types.VIEW3D_MT_mesh_add.append(add_object_button)


def unregister():
    bpy.utils.unregister_class(OBJECT_OT_add_object)
    bpy.utils.unregister_manual_map(add_object_manual_map)
    bpy.types.VIEW3D_MT_mesh_add.remove(add_object_button)


if __name__ == "__main__":
    register()
