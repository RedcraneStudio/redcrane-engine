# Copyright (C) 2016 Luke San Antonio Bialecki
# All rights reserved.

import sys
import json
import base64
import struct
import pdb

import numpy as np

RGB_FORMAT = 6407
RGB_ALPHA_FORMAT = 6408

SRGB_FORMAT = 0x8C40
SRGB_ALPHA_FORMAT = 0x8C42

TEXTURE_2D_TARGET = 3553

UNSIGNED_BYTE_TYPE = 5121

CLAMP_TO_EDGE = 33071

LINEAR_MIPMAP_LINEAR = 9987
LINEAR = 9729

OBJECTS_NAME = 'nodes'
MESHES_NAME = 'meshes'
ACCESSORS_NAME = 'accessors'
IMAGES_NAME = 'images'
TEXTURES_NAME = 'textures'
SAMPLERS_NAME = 'samplers'
MATERIALS_NAME = 'materials'
BUFFER_VIEWS_NAME = 'bufferViews'
BUFFERS_NAME = 'buffers'
LAMPS_NAME = 'lights'

BASE64_DATA_HEADER = 'data:text/plain;base64,'

def get_buffer_view(gltf, buffer_view_name):

    # Get the buffer
    buffer_view = gltf[BUFFER_VIEWS_NAME][buffer_view_name]

    buf_offset = buffer_view['byteOffset']
    buf_length = buffer_view['byteLength']

    buffer = gltf[BUFFERS_NAME][buffer_view['buffer']]

    # Handle embedded data
    data = None
    if BASE64_DATA_HEADER == buffer['uri'][:len(BASE64_DATA_HEADER)]:
        data = base64.b64decode(buffer['uri'][23:])

    return data[buf_offset:buf_offset + buf_length]

def set_fake_mesh(gltf, obj_name, vertices, indices):

    # Build the transformation matrix of this mesh
    node_mat = np.identity(4)

    # For each node in our hiearchy, basically.
    this_node = obj_name
    while this_node != "":
        # Build the total transformation matrix
        obj = gltf[OBJECTS_NAME][this_node]

        # Although the matrix is technically stored in column major order, numpy
        # expects row-major so just role with it, inverting the order of all
        # transformations.
        local_mat = np.array(obj['matrix']).reshape((4,4))

        node_mat = np.dot(node_mat, local_mat)

        # Find this node's parent
        parent_node = ""
        for node_name, node_obj in gltf[OBJECTS_NAME].items():
            if this_node in node_obj['children']:
                # We have our parent
                parent_node = node_name

        this_node = parent_node

    obj = gltf[OBJECTS_NAME][obj_name]

    # Find the mesh that this node (sorta) contains.
    mesh_name = obj['meshes'][0]

    # Remove the mesh
    old_mesh = gltf[MESHES_NAME].pop(mesh_name)

    if len(old_mesh['primitives']) > 1:
        raise RuntimeError(("Fake mesh {} must only have one primitive, does"
                            " the material have more than one"
                            " material?").format(mesh))

    prim = old_mesh['primitives'][0]

    old_vertices_name = prim['attributes']['POSITION']
    old_indices_name = prim['indices']

    # Remove normals
    old_normals_name = prim['attributes'].get('NORMALS', '')
    if old_normals_name != '':
        gltf[ACCESSORS_NAME].pop(old_normals_name)

    # Change accessor names
    vertices_obj = gltf[ACCESSORS_NAME].pop(old_vertices_name)
    indices_obj = gltf[ACCESSORS_NAME].pop(old_indices_name)

    # Remove normals

    gltf[ACCESSORS_NAME].update({vertices: vertices_obj})
    gltf[ACCESSORS_NAME].update({indices: indices_obj})

    offset = vertices_obj['byteOffset']
    stride = vertices_obj['byteStride']
    count = vertices_obj['count']

    vertices_data_view = get_buffer_view(gltf, vertices_obj['bufferView'])
    if vertices_data_view == None:
        raise RuntimeError(('Failed to find vertices data for'
                            ' mesh {}').format(mesh))

    #pdb.set_trace()
    vertices_data = vertices_data_view[offset:]
    # TODO: We assume 3 four byte floats per position value
    out_vertices_data = bytearray(count * 3 * 4)

    for i in range(count):
        # Get the vertex data
        x, y, z = struct.unpack_from('<fff', vertices_data, i * stride)

        # Transform
        new_pt = np.dot(np.array([x, y, z, 1.0]), node_mat)

        # Repack
        struct.pack_into('<fff', out_vertices_data, i * 3 * 4, new_pt[0],
                         new_pt[1], new_pt[2])

    # Make a new buffer for CPU data, make a new buffer view and finally have
    # the new vertices accessor reference the buffer view.
    buffer_name = vertices + '_buffer'
    gltf[BUFFERS_NAME][buffer_name] = {
        'byteLength': len(out_vertices_data),
        'uri': BASE64_DATA_HEADER + base64.b64encode(out_vertices_data).decode('ascii')
    }
    buffer_view_name = vertices + '_buffer_view'
    gltf[BUFFER_VIEWS_NAME][buffer_view_name] = {
        'buffer': buffer_name,
        'byteLength': len(out_vertices_data),
        'byteOffset': 0
    }
    gltf[ACCESSORS_NAME][vertices]['bufferView'] = buffer_view_name
    gltf[ACCESSORS_NAME][vertices]['byteOffset'] = 0
    gltf[ACCESSORS_NAME][vertices]['byteStride'] = 3 * 4


    # Remove the object
    gltf[OBJECTS_NAME].pop(obj_name)

    # And all references to it!
    for node_name, node in gltf[OBJECTS_NAME].items():
        if obj_name in node['children']:
            node['children'].remove(obj_name)

def remove_unused_accessors(gltf, save_these = []):
    used_accessors = []
    for mesh_name, mesh in gltf[MESHES_NAME].items():
        for prim in mesh['primitives']:
            these_accessors = []
            these_accessors.append(prim['indices'])
            these_accessors.extend(prim['attributes'].values())
            for access in these_accessors:
                if access not in used_accessors:
                    used_accessors.append(access)
    rm_accessors = []
    for name, access in gltf[ACCESSORS_NAME].items():
        if name not in used_accessors or name not in save_these:
            rm_accessors.append(name)
    for buf in rm_accessors:
        del gltf[ACCESSORS_NAME][buf]

def remove_unused_buffer_views(gltf):
    used_buffers = []
    for name, accessor in gltf[ACCESSORS_NAME].items():
        used_buffers.append(accessor['bufferView'])
    rm_buffers = []
    for buf_name, buf in gltf[BUFFER_VIEWS_NAME].items():
        if buf_name not in used_buffers:
            rm_buffers.append(buf_name)
    for buf in rm_buffers:
        del gltf[BUFFER_VIEWS_NAME][buf]

def remove_unused_buffers(gltf):
    used_buffers = []
    for name, buffer_view in gltf[BUFFER_VIEWS_NAME].items():
        used_buffers.append(buffer_view['buffer'])
    rm_buffers = []
    for buf_name, buf in gltf[BUFFERS_NAME].items():
        if buf_name not in used_buffers:
            rm_buffers.append(buf_name)
    for buf in rm_buffers:
        del gltf[BUFFERS_NAME][buf]

def remove_unused_data(gltf, save_accessors = []):
    remove_unused_accessors(gltf, save_accessors)
    remove_unused_buffer_views(gltf)
    remove_unused_buffers(gltf)

def add_textures(gltf, textures, sampler):
    if TEXTURES_NAME not in gltf:
        gltf[TEXTURES_NAME] = {}
    for tex, image in textures.items():
        tex_gltf = {}
        tex_gltf['format'] = RGB_ALPHA_FORMAT
        tex_gltf['internalFormat'] = SRGB_ALPHA_FORMAT
        tex_gltf['sampler'] = sampler
        tex_gltf['source'] = image
        tex_gltf['target'] = TEXTURE_2D_TARGET
        tex_gltf['type'] = UNSIGNED_BYTE_TYPE
        gltf[TEXTURES_NAME].update({tex: tex_gltf})

def add_images(gltf, images, image_dir):
    if IMAGES_NAME not in gltf:
        gltf[IMAGES_NAME] = {}
    for image, url in images.items():
        image_gltf = {}
        image_gltf['uri'] = image_dir + '/' + url
        gltf[IMAGES_NAME].update({image: image_gltf})

def add_lightmap_sampler(gltf, name):
    sampler = {}
    sampler['magFilter'] = LINEAR
    sampler['minFilter'] = LINEAR_MIPMAP_LINEAR
    sampler['wrapS'] = CLAMP_TO_EDGE
    sampler['wrapT'] = CLAMP_TO_EDGE
    gltf[SAMPLERS_NAME][name] = sampler

def make_unique_materials(gltf, mesh):
    for prim in mesh['primitives']:
        # Copy the material
        mat_name = prim['material']
        material = gltf[MATERIALS_NAME][mat_name]

        # Add a new material with a new name
        new_name = ''
        for i in range(1,999):
            new_name = mat_name + '.' + str(i)
            if new_name not in gltf[MATERIALS_NAME]:
                break

        # Replace this primitive with that material
        gltf[MATERIALS_NAME][new_name] = material.copy()

        prim['material'] = new_name

def get_mesh(gltf, obj, i = 0):
    meshes = obj.get('meshes', [])
    # Too few meshes
    if i >= len(meshes):
        raise RuntimeError("Object doesn't have a mesh at index {}".format(i))

    mesh = meshes[i]
    return gltf[MESHES_NAME][mesh]

def set_diffusemap(gltf, obj, lightmap):
    mesh_name = obj['meshes'][0]
    mesh = gltf[MESHES_NAME][mesh_name]

    for primitive in mesh['primitives']:
        mat_name = primitive['material']
        mat = gltf[MATERIALS_NAME][mat_name]

        # This has the effect of removing most values for the given material.
        mat['values'] = {'lightmap': lightmap}

    set_technique(gltf, obj, 'forward_diffusemap')

def get_material(gltf, prim):
    mat_name = prim.get('material', '')
    if mat_name == '': return None
    return gltf[MATERIALS_NAME][mat_name]

def remove_material_values(gltf, mesh, rm_names):
    for prim in mesh['primitives']:
        mat = get_material(gltf, prim)
        if mat == None: continue

        values = mat['values']
        for name in rm_names:
            if name in values:
                del values[name]

def adjust_shininess(gltf, mesh, name):
    for prim in mesh['primitives']:
        mat = get_material(gltf, prim)
        if mat == None: continue

        values = mat['values']
        if name in values:
            shiny = values[name] / 50.0 * 16.0
            if shiny > 1.0:
                values[name] = shiny

def set_technique(gltf, obj, technique):
    mesh = get_mesh(gltf, obj)
    for prim in mesh['primitives']:
        mat = get_material(gltf, prim)
        if mat == None:
            continue

        mat['technique'] = technique

def remove_texcoords(gltf, mesh):
    for prim in mesh['primitives']:
        rm_names = []
        for attrib_semantic, attrib_value in prim['attributes'].items():
            # String matching!
            if "TEXCOORD" in attrib_semantic:
                rm_names.append(attrib_semantic)

        for name in rm_names:
            del prim['attributes'][name]

def update_fucked_texcoords(gltf, mesh):
    for prim in mesh['primitives']:
        if 'TEXCOORD_UVMap' in prim['attributes']:
            old_mapping = prim['attributes'].pop('TEXCOORD_UVMap')
            prim['attributes']['TEXCOORD_0'] = old_mapping

def remove_unused_materials(gltf):
    mats_used = []
    for mesh_name, mesh in gltf[MESHES_NAME].items():
        for prim in mesh['primitives']:
            mat_name = prim['material']
            if mat_name not in mats_used:
                mats_used.append(mat_name)

    rm_mats = []
    for mat in gltf[MATERIALS_NAME]:
        if mat not in mats_used:
            rm_mats.append(mat)

    for mat in rm_mats:
        del gltf[MATERIALS_NAME][mat]

def remove_unmaterialed_meshes(gltf):
    for mesh_name, mesh in gltf[MESHES_NAME].items():
        rm_prims = []
        for i, prim in enumerate(mesh['primitives']):
            if prim.get('material', '') == '':
                rm_prims.append(i)
        for prim_i in rm_prims:
            del mesh['primitives'][prim_i]

def set_lamps_state(gltf, lamps, state):
    for key, lamp in gltf['extras'][LAMPS_NAME].items():
        if key in lamps:
            lamp['active'] = state

def turn_lamps_off(gltf, lamps):
    set_lamps_state(gltf, lamps, False)

def turn_lamps_on(gltf, lamps):
    set_lamps_state(gltf, lamps, True)

if __name__ == '__main__':

    if len(sys.argv) < 3:
        sys.stderr.write('usage: ' + sys.argv[0] + ' <map.gltf> <out.gltf>\n')
        sys.exit()

    # Load glTF
    with open(sys.argv[1]) as f:
        gltf = json.load(f)

    set_fake_mesh(gltf, 'Room', 'Collision_Vertices', 'Collision_Indices')

    for obj_name, obj in gltf[OBJECTS_NAME].items():
        try:
            mesh = get_mesh(gltf, obj)
        except RuntimeError:
            continue
        except KeyError:
            raise

        remove_material_values(gltf, mesh, ['specular', 'emission',
                                           'ambient', 'uv_layers', 'textures'])

        adjust_shininess(gltf, mesh, 'shininess')
        remove_texcoords(gltf, mesh)

        # Set dynamic lighting
        set_technique(gltf, obj, 'deferred_dynamic_lights')

    remove_unused_materials(gltf)
    remove_unmaterialed_meshes(gltf)

    turn_lamps_off(gltf, ['Left_Lamp_Spot', 'Night_Light', 'Right_Lamp_Spot'])
    turn_lamps_on(gltf, ['Desk_Lamp'])

    #remove_unused_data(gltf, ['Collision_Vertices', 'Collision_Indices'])

    with open(sys.argv[2], 'w') as f:
        json.dump(gltf, f, indent=4)
        f.write('\n')
