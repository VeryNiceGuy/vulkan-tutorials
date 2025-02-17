#include "obj_loader.h"
#include "string_utils.h"
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MESH_ELEMENT_ARRAY_LENGTH 1024
#define OBJECT_ARRAY_LENGTH 64

void load_mtl(const char* file_path, DynamicArray* materials, Hashtable* material_map) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Failed to open MTL file: %s\n", file_path);
        return;
    }

    char line[256];
    ObjMaterial* material = NULL;

    while (fgets(line, sizeof(line), file)) {
        trim_start(line);

        if (strncmp(line, "newmtl ", 7) == 0) {
            material = get_next_ptr(materials);
            sscanf(line, "newmtl %127s", material->name);
            insert_hashtable(material_map, material->name, materials->size - 1);

            material->ambient.r = material->ambient.g = material->ambient.b = 0.2f;
            material->diffuse.r = material->diffuse.g = material->diffuse.b = 0.8f;
            material->specular.r = material->specular.g = material->specular.b = 1.0f;
            material->emissive.r = material->emissive.g = material->emissive.b = 0.0f;
            material->glossiness = 0.0f;
            material->opacity = 1.0f;
            material->transparency = 1.0f;
            material->illumination_model = 2;
            material->ambient_map_filepath[0] = '\0';
            material->diffuse_map_filepath[0] = '\0';
            material->specular_map_filepath[0] = '\0';
            material->emissive_map_filepath[0] = '\0';
            material->alpha_map_filepath[0] = '\0';
            material->bump_map_filepath[0] = '\0';
            material->displacement_map_filepath[0] = '\0';
        }
        else if (strncmp(line, "Ka ", 3) == 0 && material) {
            sscanf(line, "Ka %f %f %f", &material->ambient.r, &material->ambient.g, &material->ambient.b);
        }
        else if (strncmp(line, "Kd ", 3) == 0 && material) {
            sscanf(line, "Kd %f %f %f", &material->diffuse.r, &material->diffuse.g, &material->diffuse.b);
        }
        else if (strncmp(line, "Ks ", 3) == 0 && material) {
            sscanf(line, "Ks %f %f %f", &material->specular.r, &material->specular.g, &material->specular.b);
        }
        else if (strncmp(line, "Ke ", 3) == 0 && material) {
            sscanf(line, "Ke %f %f %f", &material->emissive.r, &material->emissive.g, &material->emissive.b);
        }
        else if (strncmp(line, "Ns ", 3) == 0 && material) {
            sscanf(line, "Ns %f", &material->glossiness);
        }
        else if (strncmp(line, "Ni ", 3) == 0 && material) {
            sscanf(line, "Ni %f", &material->opacity);
        }
        else if (strncmp(line, "d ", 2) == 0 && material) {
            sscanf(line, "d %f", &material->transparency);
        }
        else if (strncmp(line, "illum ", 6) == 0 && material) {
            sscanf(line, "illum %d", &material->illumination_model);
        }
        else if (strncmp(line, "map_Ka ", 7) == 0 && material) {
            sscanf(line, "map_Ka %255s", material->ambient_map_filepath);
        }
        else if (strncmp(line, "map_Kd ", 7) == 0 && material) {
            sscanf(line, "map_Kd %255s", material->diffuse_map_filepath);
        }
        else if (strncmp(line, "map_Ks ", 7) == 0 && material) {
            sscanf(line, "map_Ks %255s", material->specular_map_filepath);
        }
        else if (strncmp(line, "map_Ke ", 7) == 0 && material) {
            sscanf(line, "map_Ke %255s", material->emissive_map_filepath);
        }
        else if (strncmp(line, "map_d ", 6) == 0 && material) {
            sscanf(line, "map_d %255s", material->alpha_map_filepath);
        }
        else if (strncmp(line, "map_Bump ", 9) == 0 && material) {
            sscanf(line, "map_Bump %255s", material->bump_map_filepath);
        }
        else if (strncmp(line, "bump ", 5) == 0 && material) {
            sscanf(line, "bump %255s", material->bump_map_filepath);
        }
        else if (strncmp(line, "disp ", 5) == 0 && material) {
            sscanf(line, "disp %255s", material->displacement_map_filepath);
        }
    }

    fclose(file);
}

void parse_vertex(DynamicArray* array, const char* const line) {
    ObjVertex* vertex = get_next_ptr(array);
    sscanf(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
}

void parse_texcoord(DynamicArray* array, const char* const line) {
    ObjTexCoord* texcoord = get_next_ptr(array);
    sscanf(line, "vt %f %f", &texcoord->u, &texcoord->v);
}

void parse_normal(DynamicArray* array, const char* const line) {
    ObjNormal* normal = get_next_ptr(array);
    sscanf(line, "vn %f %f %f", &normal->x, &normal->y, &normal->z);
}

bool is_absolute_indexing(int face_index) {
    return face_index >= 0;
}

void adjust_indices(ObjFace* face, uint32_t vertex_count) {
    if (is_absolute_indexing(face->vertex_indices[0])) {
        face->vertex_indices[0] -= 1;
        face->vertex_indices[1] -= 1;
        face->vertex_indices[2] -= 1;

        if (face->has_texcoords) {
            face->texcoord_indices[0] -= 1;
            face->texcoord_indices[1] -= 1;
            face->texcoord_indices[2] -= 1;
        }

        if (face->has_normals) {
            face->normal_indices[0] -= 1;
            face->normal_indices[1] -= 1;
            face->normal_indices[2] -= 1;
        }
    }
    else {
        face->vertex_indices[0] += vertex_count;
        face->vertex_indices[1] += vertex_count;
        face->vertex_indices[2] += vertex_count;

        if (face->has_texcoords) {
            face->texcoord_indices[0] += vertex_count;
            face->texcoord_indices[1] += vertex_count;
            face->texcoord_indices[2] += vertex_count;
        }

        if (face->has_normals) {
            face->normal_indices[0] += vertex_count;
            face->normal_indices[1] += vertex_count;
            face->normal_indices[2] += vertex_count;
        }
    }
}

void parse_face(
    DynamicArray* array,
    Hashtable* hashtable,
    char* material_name,
    uint32_t vertex_count,
    const char* const line) {

    size_t material_index;
    bool material_exists = search_hashtable(hashtable, material_name, &material_index);

    ObjMaterialFaceGroup* material_face_group;
    if (material_exists) {
        material_face_group = get_element_ptr(array, material_index);
    } else {
        insert_hashtable(hashtable, material_name, array->size);
        material_face_group = get_next_ptr(array);
        strcpy(material_face_group->material_name, material_name);
        init_dynamic_array(&material_face_group->faces, sizeof(ObjFace), MESH_ELEMENT_ARRAY_LENGTH);
    }

    ObjFace* face = get_next_ptr(&material_face_group->faces);
    int count = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
        &face->vertex_indices[0],
        &face->texcoord_indices[0],
        &face->normal_indices[0],
        &face->vertex_indices[1],
        &face->texcoord_indices[1],
        &face->normal_indices[1],
        &face->vertex_indices[2],
        &face->texcoord_indices[2],
        &face->normal_indices[2]
    );

    if (count == 9) {
        face->has_texcoords = true;
        face->has_normals = true;
    } else {
        count = sscanf(line, "f %d//%d %d//%d %d//%d",
            &face->vertex_indices[0], &face->normal_indices[0],
            &face->vertex_indices[1], &face->normal_indices[1],
            &face->vertex_indices[2], &face->normal_indices[2]);

        if (count == 6) {
            face->has_normals = true;
        } else {
            count = sscanf(line, "f %d/%d %d/%d %d/%d",
                &face->vertex_indices[0], &face->texcoord_indices[0],
                &face->vertex_indices[1], &face->texcoord_indices[1],
                &face->vertex_indices[2], &face->texcoord_indices[2]);

            if (count == 6) {
                face->has_texcoords = true;
            } else {
                sscanf(line, "f %d %d %d",
                    &face->vertex_indices[0],
                    &face->vertex_indices[1],
                    &face->vertex_indices[2]);
            }
        }
    }

    adjust_indices(face, vertex_count);
}

void parse_mtllib(char* str, const char* const line) {
    sscanf(line, "mtllib %255s", str);
}

ObjGroup* parse_group(DynamicArray* array, const char* const line) {
    ObjGroup* group = get_next_ptr(array);
    sscanf(line, "g %127s", group->name);

    init_dynamic_array(&group->material_face_groups, sizeof(ObjMaterialFaceGroup), OBJECT_ARRAY_LENGTH);
    init_hashtable(&group->material_face_group_map, OBJECT_ARRAY_LENGTH);

    return group;
}

ObjObject* parse_object(DynamicArray* array, const char* const line) {
    ObjObject* object = get_next_ptr(array);
    sscanf(line, "o %127s", object->name);

    init_dynamic_array(&object->material_face_groups, sizeof(ObjMaterialFaceGroup), OBJECT_ARRAY_LENGTH);
    init_hashtable(&object->material_face_group_map, OBJECT_ARRAY_LENGTH);
    init_dynamic_array(&object->groups, sizeof(ObjGroup), OBJECT_ARRAY_LENGTH);

    return object;
}

void parse_usemtl(char* material_name, const char* const line) {
    sscanf(line, "usemtl %127s", material_name);
}

void load_obj_model(const char* file_path, ObjModel* model) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Failed to open OBJ file: %s\n", file_path);
        return;
    }

    char line[256];
    char mtl_file[256];

    ObjObject* current_object = NULL;
    ObjGroup* current_group = NULL;
    char current_material_name[128] = "no_material";

    init_dynamic_array(&model->vertices, sizeof(ObjVertex), MESH_ELEMENT_ARRAY_LENGTH);
    init_dynamic_array(&model->texcoords, sizeof(ObjTexCoord), MESH_ELEMENT_ARRAY_LENGTH);
    init_dynamic_array(&model->normals, sizeof(ObjNormal), MESH_ELEMENT_ARRAY_LENGTH);
    init_dynamic_array(&model->material_face_groups, sizeof(ObjMaterialFaceGroup), OBJECT_ARRAY_LENGTH);
    init_hashtable(&model->material_face_group_map, OBJECT_ARRAY_LENGTH);
    init_dynamic_array(&model->objects, sizeof(ObjObject), OBJECT_ARRAY_LENGTH);
    init_dynamic_array(&model->groups, sizeof(ObjGroup), OBJECT_ARRAY_LENGTH);
    init_dynamic_array(&model->materials, sizeof(ObjMaterial), OBJECT_ARRAY_LENGTH);
    init_hashtable(&model->material_map, OBJECT_ARRAY_LENGTH);

    while (fgets(line, sizeof(line), file)) {
        trim_start(line);

        if (strncmp(line, "mtllib ", 7) == 0) {
            parse_mtllib(mtl_file, line);
            load_mtl(mtl_file, &model->materials, &model->material_map);
        }
        else if (strncmp(line, "o ", 2) == 0) {
            current_object = parse_object(&model->objects, line);
            current_group = NULL;
        }
        else if (strncmp(line, "g ", 2) == 0) {
            current_group = parse_group(current_object ? &current_object->groups : &model->groups, line);
        }
        else if (strncmp(line, "v ", 2) == 0) {
            parse_vertex(&model->vertices, line);
        }
        else if (strncmp(line, "vt ", 3) == 0) {
            parse_texcoord(&model->texcoords, line);
        }
        else if (strncmp(line, "vn ", 3) == 0) {
            parse_normal(&model->normals, line);
        }
        else if (strncmp(line, "f ", 2) == 0) {
            if (current_group) {
                parse_face(
                    &current_group->material_face_groups,
                    &current_group->material_face_group_map,
                    current_material_name,
                    model->vertices.size,
                    line);
            }
            else if (current_object){
                parse_face(
                    &current_object->material_face_groups,
                    &current_object->material_face_group_map,
                    current_material_name,
                    model->vertices.size,
                    line);
            }
            else {
                parse_face(
                    &model->material_face_groups,
                    &model->material_face_group_map,
                    current_material_name,
                    model->vertices.size,
                    line);
            }
        }
        else if (strncmp(line, "usemtl ", 7) == 0) {
            parse_usemtl(current_material_name, line);
        }
    }

    fclose(file);

    for (size_t i = 0; i < model->material_face_groups.size; ++i) {
        ObjMaterialFaceGroup* material_face_group = get_element_ptr(&model->material_face_groups, i);

        if (search_hashtable(&model->material_map, material_face_group->material_name, NULL)) {
            material_face_group->has_material = true;
        } else {
            material_face_group->has_material = false;
        }
    }

    for (size_t k = 0; k < model->groups.size; ++k) {
        ObjGroup* group = get_element_ptr(&model->groups, k);

        for (size_t i = 0; i < group->material_face_groups.size; ++i) {
            ObjMaterialFaceGroup* material_face_group = get_element_ptr(&group->material_face_groups, i);

            if (search_hashtable(&model->material_map, material_face_group->material_name, NULL)) {
                material_face_group->has_material = true;
            }
            else {
                material_face_group->has_material = false;
            }
        }
    }

    for (size_t j = 0; j < model->objects.size; ++j) {
        ObjObject* object = get_element_ptr(&model->objects, j);

        for (size_t t = 0; t < object->material_face_groups.size; ++t) {
            ObjMaterialFaceGroup* material_face_group = get_element_ptr(&object->material_face_groups, t);
            
            if (search_hashtable(&model->material_map, material_face_group->material_name, NULL)) {
                material_face_group->has_material = true;
            }
            else {
                material_face_group->has_material = false;
            }
        }

        for (size_t r = 0; r < object->groups.size; ++r) {
            ObjGroup* group = get_element_ptr(&object->groups, r);

            for (size_t i = 0; i < group->material_face_groups.size; ++i) {
                ObjMaterialFaceGroup* material_face_group = get_element_ptr(&group->material_face_groups, i);

                if (search_hashtable(&model->material_map, material_face_group->material_name, NULL)) {
                    material_face_group->has_material = true;
                }
                else {
                    material_face_group->has_material = false;
                }
            }
        }
    }
}

void free_obj_model(ObjModel* model) {
    free_dynamic_array(&model->vertices);
    free_dynamic_array(&model->texcoords);
    free_dynamic_array(&model->texcoords);
    free_dynamic_array(&model->material_face_groups);
    free_hashtable(&model->material_face_group_map);
    free_dynamic_array(&model->materials);

    for (size_t k = 0; k < model->groups.size; ++k) {
        ObjGroup* group = get_element_ptr(&model->groups, k);

        for (size_t t = 0; t < group->material_face_groups.size; ++t) {
            ObjMaterialFaceGroup* material_face_group = get_element_ptr(&group->material_face_groups, t);
            free_dynamic_array(&material_face_group->faces);
        }

        free_dynamic_array(&group->material_face_groups);
        free_hashtable(&group->material_face_group_map);
    }

    free_dynamic_array(&model->groups);

    for (size_t i = 0; i < model->objects.size; ++i) {
        ObjObject* object = get_element_ptr(&model->objects, i);

        for (size_t t = 0; t < object->material_face_groups.size; ++t) {
            ObjMaterialFaceGroup* material_face_group = get_element_ptr(&object->material_face_groups, t);
            free_dynamic_array(&material_face_group->faces);
        }

        free_dynamic_array(&object->material_face_groups);
        
        for (size_t j = 0; j < object->groups.size; ++j) {
            ObjGroup* group = get_element_ptr(&object->groups, i);

            for (size_t t = 0; t < group->material_face_groups.size; ++t) {
                ObjMaterialFaceGroup* material_face_group = get_element_ptr(&group->material_face_groups, t);
                free_dynamic_array(&material_face_group->faces);
            }

            free_dynamic_array(&group->material_face_groups);
        }

        free_dynamic_array(&object->groups);
    }

    free_dynamic_array(&model->objects);
}
