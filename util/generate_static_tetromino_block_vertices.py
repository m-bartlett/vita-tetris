#!/usr/bin/env python3

TETROMINO_ROTATIONS = {
  "TETROMINO_TYPE_I" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '1111'
                                   '0000'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0010'
                                   '0010'
                                   '0010'
                                   '0010',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0000'
                                   '1111'
                                   '0000',2),
    "TETROMINO_ROTATION_270" : int('0100'
                                   '0100'
                                   '0100'
                                   '0100',2),
  },
  "TETROMINO_TYPE_O" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '0110'
                                   '0110'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0000'
                                   '0110'
                                   '0110'
                                   '0000',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0110'
                                   '0110'
                                   '0000',2),
    "TETROMINO_ROTATION_270" : int('0000'
                                   '0110'
                                   '0110'
                                   '0000',2)
  },
  "TETROMINO_TYPE_T" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '0100'
                                   '1110'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0000'
                                   '0100'
                                   '0110'
                                   '0100',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0000'
                                   '1110'
                                   '0100',2),
    "TETROMINO_ROTATION_270" : int('0000'
                                   '0100'
                                   '1100'
                                   '0100',2)
  },
  "TETROMINO_TYPE_J" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '1000'
                                   '1110'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0000'
                                   '0110'
                                   '0100'
                                   '0100',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0000'
                                   '1110'
                                   '0010',2),
    "TETROMINO_ROTATION_270" : int('0000'
                                   '0100'
                                   '0100'
                                   '1100',2)
  },
  "TETROMINO_TYPE_L" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '0010'
                                   '1110'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0000'
                                   '0100'
                                   '0100'
                                   '0110',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0000'
                                   '1110'
                                   '1000',2),
    "TETROMINO_ROTATION_270" : int('0000'
                                   '1100'
                                   '0100'
                                   '0100',2)
  },
  "TETROMINO_TYPE_S" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '0110'
                                   '1100'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0000'
                                   '0100'
                                   '0110'
                                   '0010',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0000'
                                   '0110'
                                   '1100',2),
    "TETROMINO_ROTATION_270" : int('0000'
                                   '1000'
                                   '1100'
                                   '0100',2)
  },
  "TETROMINO_TYPE_Z" : {
    "TETROMINO_ROTATION_0"   : int('0000'
                                   '1100'
                                   '0110'
                                   '0000',2),
    "TETROMINO_ROTATION_90"  : int('0000'
                                   '0010'
                                   '0110'
                                   '0100',2),
    "TETROMINO_ROTATION_180" : int('0000'
                                   '0000'
                                   '1100'
                                   '0110',2),
    "TETROMINO_ROTATION_270" : int('0000'
                                   '0100'
                                   '1100'
                                   '1000',2)
  },
}


def add_vertex(x, y, z, u, v, block, face):
  return {"x":x, "y":y, "z":z, "u":u, "v":v, "face":face}

def vertex_to_string(x, y, z, u, v, face):
  return f"{{.x={x}, .y={y}, .z={z}, .u={u}, .v={v}, .face={face}}}"

def add_front_face(x, y, block):
  x1, y1 = x+1, y+1
  return [
    add_vertex(x,  y,  1, 1, 1, block, "FACE_FRONT"),
    add_vertex(x,  y1, 1, 1, 2, block, "FACE_FRONT"),
    add_vertex(x1, y1, 1, 2, 2, block, "FACE_FRONT"),
    add_vertex(x1, y,  1, 2, 1, block, "FACE_FRONT"),
  ]

def add_left_face(x, y, block):
  x1, y1 = x+1, y+1
  return [
    add_vertex(x,  y,  0, 0, 1, block, "FACE_LEFT"),
    add_vertex(x,  y1, 0, 0, 2, block, "FACE_LEFT"),
    add_vertex(x,  y1, 1, 1, 2, block, "FACE_LEFT"),
    add_vertex(x,  y,  1, 1, 1, block, "FACE_LEFT"),
  ]

def add_right_face(x, y, block):
  x1, y1 = x+1, y+1
  return [
    add_vertex(x1, y,  1, 2, 1, block, "FACE_RIGHT"),
    add_vertex(x1, y1, 1, 2, 2, block, "FACE_RIGHT"),
    add_vertex(x1, y1, 0, 3, 2, block, "FACE_RIGHT"),
    add_vertex(x1, y,  0, 3, 1, block, "FACE_RIGHT"),
  ]

def add_top_face(x, y, block):
  x1, y1 = x+1, y+1
  return [
    add_vertex(x,  y1, 1, 1, 2, block, "FACE_TOP"),
    add_vertex(x,  y1, 0, 1, 3, block, "FACE_TOP"),
    add_vertex(x1, y1, 0, 2, 3, block, "FACE_TOP"),
    add_vertex(x1, y1, 1, 2, 2, block, "FACE_TOP"),
  ]

def add_bottom_face(x, y, block):
  x1, y1 = x+1, y+1
  return [
    add_vertex(x,  y,  0, 1, 0, block, "FACE_BOTTOM"),
    add_vertex(x,  y,  1, 1, 1, block, "FACE_BOTTOM"),
    add_vertex(x1, y,  1, 2, 1, block, "FACE_BOTTOM"),
    add_vertex(x1, y,  0, 2, 0, block, "FACE_BOTTOM"),
  ]


#####################################################################################

get_maskbit = lambda x,y: 1<<(y*4+(3-x))
print_maskbit = lambda m: print(bin(m)[2:].rjust(16,'0'))


def scan_left_to_right(grid, block):
  lines = []

  previous_occupied = False
  for y in range(4):
    previous_occupied = False
    for x in range(4):
      maskbit = get_maskbit(x, y);
      current_occupied = maskbit&grid

      if previous_occupied and current_occupied:
        lines += add_front_face(x, y, block)

      elif not previous_occupied and current_occupied:
        lines += add_left_face(x, y, block)
        lines += add_front_face(x, y, block)

      elif previous_occupied and not current_occupied:
        lines += add_right_face(x-1, y, block)

      elif not previous_occupied and not current_occupied:
        pass
          
      previous_occupied = current_occupied

    if previous_occupied:
      lines += add_right_face(x-1, y, block)

  return lines


def scan_top_to_bottom(grid, block):
  lines = []

  # 00[0] 10[0]   [0]   [0]
  # 00[0] 11[1]   [1]   [0]
  # 01[1]*01[1]   [0]   [0]
  # 10[0] 00[0]   [0]   [0]

  for x in range(4):
    previous_occupied = False
    for y in range(4):
      maskbit = get_maskbit(x, y);
      current_occupied = maskbit&grid

      if previous_occupied and current_occupied:
        pass

      elif not previous_occupied and current_occupied:
        lines += add_bottom_face(x, y, block)

      elif previous_occupied and not current_occupied:
        lines += add_top_face(x, y-1, block)

      elif not previous_occupied and not current_occupied:
        pass

      previous_occupied = current_occupied

    if previous_occupied:
      lines += add_bottom_face(x, y, block)

  return lines


def grid_to_face_vertices(grid, block):
  return scan_left_to_right(grid, block) + scan_top_to_bottom(grid, block)


TETROMINO_MESHES = {
  tetromino_type: {
    rotation: grid_to_face_vertices(grid, tetromino_type)
    for rotation, grid in rotations.items()
  }
  for tetromino_type, rotations in TETROMINO_ROTATIONS.items()
}


total_size=0
for tetromino_type, rotations in TETROMINO_MESHES.items():
  for rotation, vertices in rotations.items():
    total_size += len(vertices) * len(vertices[0].values())
print(f"// total size of static tetromino meshes == {total_size} bytes")

indent_prefix='  '

for tetromino_type, rotations in TETROMINO_MESHES.items():
  outer_name = f"{tetromino_type}_MESH"
  vertex_quantity = len(rotations["TETROMINO_ROTATION_0"])
  # print(f"\n#define {outer_name}_SIZE {vertex_quantity}")
  print(f"const graphics_block_ROM_vertex_t\n{outer_name}ES[4][{outer_name}_SIZE] = {{")
  for rotation, vertices in rotations.items():
    print(f"{indent_prefix}[{rotation}] = {{")
    total_size += vertex_quantity

    vertex_lines = [vertex_to_string(**v) for v in vertices]

    print(indent_prefix*2, end='')
    print(f",\n{indent_prefix*2}".join(vertex_lines))
    print(f"{indent_prefix}}},")
  print("};")



# graphics_block_vertex_t* grpahics_tetromino_get_rotated_mesh(tetromino_t *t) {
#     switch(t->type) {

#     }
# }
# TETROMINO_MESH[TETROMINO_TYPE_QUANTITY-1][4] = {
#     # [TETROMINO_TYPE_NULL] = {{0},{0},{0},{0}},
#     [TETROMINO_TYPE_I-1] = {
#         [TETROMINO_ROTATION_0]   = {TETROMINO_TYPE_I_MESH_ROTATED_0},
#         [TETROMINO_ROTATION_90]  = {TETROMINO_TYPE_I_MESH_ROTATED_90},
#         [TETROMINO_RO        TATION_180] = {TETROMINO_TYPE_I_MESH_ROTATED_180},
#         [TETROMINO_ROTATION_270] = {TETROMINO_TYPE_I_MESH_ROTATED_270}
#     },
#     ...
# }

# size_t TETROMINO_MESH_SIZES[TETROMINO_TYPE_QUANTITY-1][4] = {
#     # [TETROMINO_TYPE_NULL] = {{0},{0},{0},{0}},
#     [TETROMINO_TYPE_I-1] = {
#         [TETROMINO_ROTATION_0]   = {TETROMINO_TYPE_I_MESH_ROTATED_0_SIZE},
#         [TETROMINO_ROTATION_90]  = {TETROMINO_TYPE_I_MESH_ROTATED_90_SIZE},
#         [TETROMINO_ROTATION_180] = {TETROMINO_TYPE_I_MESH_ROTATED_180_SIZE},
#         [TETROMINO_ROTATION_270] = {TETROMINO_TYPE_I_MESH_ROTATED_270_SIZE}
#     },
#     ...
# }