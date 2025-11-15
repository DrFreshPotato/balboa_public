#include "hw2.h"
#include "hw2_scenes.h"
#include "hw1.h"
using namespace hw2;

Vector2 proj_screen_pt(Vector3 p, Real width, Real height, Real s, Real a){
    Vector2 p_prime = Vector2(p.x/-p.z, p.y/-p.z);
    Real x_prime = width * ((p_prime.x + s*a)/(2*s*a));
    Real y_prime = height * ((p_prime.y + s)/(2*s));

    return Vector2(x_prime, y_prime);
}

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    Vector3 bg_color = Vector3{0.5,0.5,0.5};
    Real aspect_ratio = (Real)img.width/(Real)img.height;
    Vector2 p0_prime = proj_screen_pt(p0, img.width, img.height, s, aspect_ratio);
    Vector2 p1_prime = proj_screen_pt(p1, img.width, img.height, s, aspect_ratio);
    Vector2 p2_prime = proj_screen_pt(p2, img.width, img.height, s, aspect_ratio);
    std::vector<Vector2> polyline = {p0_prime, p1_prime, p2_prime};

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 avg = bg_color;
            if (-p0.z < z_near || -p1.z < z_near || -p2.z < z_near){
                img(x,y) = bg_color;
                continue;
            }
            for (Real k = 0.125; k < 1; k+=Real(0.25)){
                for (Real j = 0.125; j < 1; j+=Real(0.25)){
                    Real pt_x = x + j;
                    Real pt_y = img.height - y + k;

                    Vector2 pt = Vector2{pt_x, pt_y};
                    
                    if (rayline_intersect(pt, polyline)){
                        avg += color;
                    }else{
                        avg += bg_color;
                    }
                }
            }
            img(x,y) = avg / Real(16);
        }
    }
    return img;
}

Vector3 proj_pt (Vector3 pt){
    return Vector3(pt.x/-pt.z, pt.y/-pt.z, -1.0);
}

Real bary_coeff(Vector3 p0, Vector3 p1, Vector3 p2, Real area){
    Vector3 v1 = p1-p0;
    Vector3 v2 = p2-p0;

    return (length(cross(v1,v2))/2)/area;
}

// Pt, Resolution, Scale, Aspect Ratio
Vector3 screenToProj(Vector2 pt, Image3 img, Real s, Real a){
    Real x = ((pt.x * (2 * s * a))/img.width) - (s*a);
    Real y = ((pt.y * (2 * s))/img.height) - s;

    return Vector3(x, y, -1.0);
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s); 
    UNUSED(z_near);
    UNUSED(scene_id);
    
    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this
    

    Real aspect_ratio = (Real)img.width/(Real)img.height;
    
    Vector3 bg_color = Vector3{0.5,0.5,0.5};
    
    double large_width = 4 * img.width;
    double large_height = 4* img.height;

    Image1 z_buffer(large_width, large_height);
    Image3 large_img(large_width, large_height); 
    for (int i = 0; i < large_width; i++){
        for (int j = 0; j < large_height; j++){
            large_img(i,j) = bg_color;
            z_buffer(i,j) = -INFINITY;
        }
    }

    // foreach Triangle
    for (int i = 0; i < mesh.faces.size(); i++){
        Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
        Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
        Vector3 p2 = mesh.vertices[mesh.faces[i][2]];
        Vector3 color = mesh.face_colors[i];

        Vector3 p0_cam = proj_pt(p0);
        Vector3 p1_cam = proj_pt(p1);
        Vector3 p2_cam = proj_pt(p2);

        Vector2 p0_prime = proj_screen_pt(p0, img.width, img.height, s, aspect_ratio);
        Vector2 p1_prime = proj_screen_pt(p1, img.width, img.height, s, aspect_ratio);
        Vector2 p2_prime = proj_screen_pt(p2, img.width, img.height, s, aspect_ratio);
        std::vector<Vector2> polyline = {p0_prime, p1_prime, p2_prime};
        Real total_area = bary_coeff(p0_cam, p1_cam, p2_cam, 1);

        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Real pt_x = x;
                Real pt_y = img.height - y;

                int b = 0;
                for (Real k = 0.125; k < 1; k+=Real(0.25)){
                    int a = 0;
                    for (Real j = 0.125; j < 1; j+=Real(0.25)){
                        Vector2 pt = Vector2{pt_x + j, pt_y + k};

                        if (rayline_intersect(pt, polyline)){
                            Vector3 camProj = screenToProj(pt, img, s, aspect_ratio);
                            
                            double b0_prime = bary_coeff(camProj, p1_cam, p2_cam, total_area);
                            double b1_prime = bary_coeff(p0_cam, camProj, p2_cam, total_area);
                            double b2_prime = bary_coeff(p0_cam, p1_cam, camProj, total_area);

                            double d = (b0_prime/p0.z) + (b1_prime/p1.z) + (b2_prime/p2.z);
                            double b0 = (b0_prime/p0.z) / d;
                            double b1 = (b1_prime/p1.z) / d;
                            double b2 = (b2_prime/p2.z) / d;

                            double p_z = (b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z);

                            double n = (4 * x) + a;
                            double m = (4 * y) + b;


                            if (p_z > z_buffer(n,m)){
                                z_buffer(n,m) = p_z;
                                large_img(n,m) = color;
                            }
                        }
                        a++;
                    }
                    b++;
                }
            }
        }
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Vector3 color = bg_color;
                double n = 4 * x;
                double m = 4 * y;
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        color += large_img(n + i, m + j);
                    }
                }
                img(x, y) = color / Real(16);
            }
        }
    
    }
    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s); 
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    Real aspect_ratio = (Real)img.width/(Real)img.height;
    
    Vector3 bg_color = Vector3{0.5,0.5,0.5};
    
    double large_width = 4 * img.width;
    double large_height = 4* img.height;

    Image1 z_buffer(large_width, large_height);
    Image3 large_img(large_width, large_height); 
    for (int i = 0; i < large_width; i++){
        for (int j = 0; j < large_height; j++){
            large_img(i,j) = bg_color;
            z_buffer(i,j) = -INFINITY;
        }
    }

    // foreach Triangle
    for (int i = 0; i < mesh.faces.size(); i++){
        Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
        Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
        Vector3 p2 = mesh.vertices[mesh.faces[i][2]];

        Vector3 p0_cam = proj_pt(p0);
        Vector3 p1_cam = proj_pt(p1);
        Vector3 p2_cam = proj_pt(p2);

        Vector2 p0_prime = proj_screen_pt(p0, img.width, img.height, s, aspect_ratio);
        Vector2 p1_prime = proj_screen_pt(p1, img.width, img.height, s, aspect_ratio);
        Vector2 p2_prime = proj_screen_pt(p2, img.width, img.height, s, aspect_ratio);
        std::vector<Vector2> polyline = {p0_prime, p1_prime, p2_prime};
        Real total_area = bary_coeff(p0_cam, p1_cam, p2_cam, 1);

        //Bounding Box
        // int x_min = std::min({p0_prime.x, p1_prime.x, p2_prime.x});
        // int x_max = std::max({p0_prime.x, p1_prime.x, p2_prime.x});
        // int y_min = std::min({p0_prime.y, p1_prime.y, p2_prime.y});
        // int y_max = std::max({p0_prime.y, p1_prime.y, p2_prime.y});

        // std::cout << x_min << "," << x_max << std::endl;
        // std::cout << y_min << "," << y_max << std::endl;

        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Real pt_x = x;
                Real pt_y = img.height - y;

                int b = 0;
                for (Real k = 0.125; k < 1; k+=Real(0.25)){
                    int a = 0;
                    for (Real j = 0.125; j < 1; j+=Real(0.25)){
                        Vector2 pt = Vector2{pt_x + j, pt_y + k};

                        if (rayline_intersect(pt, polyline)){
                            Vector3 camProj = screenToProj(pt, img, s, aspect_ratio);
                            
                            double b0_prime = bary_coeff(camProj, p1_cam, p2_cam, total_area);
                            double b1_prime = bary_coeff(p0_cam, camProj, p2_cam, total_area);
                            double b2_prime = bary_coeff(p0_cam, p1_cam, camProj, total_area);

                            double d = (b0_prime/p0.z) + (b1_prime/p1.z) + (b2_prime/p2.z);
                            double b0 = (b0_prime/p0.z) / d;
                            double b1 = (b1_prime/p1.z) / d;
                            double b2 = (b2_prime/p2.z) / d;

                            double p_z = (b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z);

                            double n = (4 * x) + a;
                            double m = (4 * y) + b;


                            if (p_z > z_buffer(n,m)){
                                z_buffer(n,m) = p_z;
                                large_img(n,m) = (b0 * mesh.vertex_colors[mesh.faces[i][0]]) + (b1 * mesh.vertex_colors[mesh.faces[i][1]]) + (b2 * mesh.vertex_colors[mesh.faces[i][2]]);
                            }
                        }
                        a++;
                    }
                    b++;
                }
            }
        }
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Vector3 color = bg_color;
                double n = 4 * x;
                double m = 4 * y;
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        color += large_img(n + i, m + j);
                    }
                }
                img(x, y) = color / Real(16);
            }
        }
    
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Real s = scene.camera.s; // scaling factor of the view frustrum
    Real z_near = scene.camera.z_near; // distance of the near clipping plane
    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);
    Vector3 bg_color = scene.background;
    Matrix4x4 V = inverse(scene.camera.cam_to_world);

    Real aspect_ratio = (Real)img.width/(Real)img.height;
    double large_width = 4 * img.width;
    double large_height = 4* img.height;

    Image1 z_buffer(large_width, large_height);
    Image3 large_img(large_width, large_height); 
    for (int i = 0; i < large_width; i++){
        for (int j = 0; j < large_height; j++){
            large_img(i,j) = bg_color;
            z_buffer(i,j) = INFINITY;
        }
    }
    for (int scene_id = 0; scene_id < scene.meshes.size(); scene_id++){
        TriangleMesh mesh = scene.meshes[scene_id];
        Matrix4x4 T = V * mesh.model_matrix;

        for (int i = 0; i < mesh.faces.size(); i++){
            Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
            Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
            Vector3 p2 = mesh.vertices[mesh.faces[i][2]];

            Vector4 t0 = T * Vector4{p0.x, p0.y, p0.z, Real(1)};
            Vector4 t1 = T * Vector4{p1.x, p1.y, p1.z, Real(1)};
            Vector4 t2 = T * Vector4{p2.x, p2.y, p2.z, Real(1)};

            Vector3 p0_cam = proj_pt(Vector3{t0.x, t0.y, t0.z});
            Vector3 p1_cam = proj_pt(Vector3{t1.x, t1.y, t1.z});
            Vector3 p2_cam = proj_pt(Vector3{t2.x, t2.y, t2.z});

            Vector2 p0_prime = proj_screen_pt(Vector3{t0.x, t0.y, t0.z}, img.width, img.height, s, aspect_ratio);
            Vector2 p1_prime = proj_screen_pt(Vector3{t1.x, t1.y, t1.z}, img.width, img.height, s, aspect_ratio);
            Vector2 p2_prime = proj_screen_pt(Vector3{t2.x, t2.y, t2.z}, img.width, img.height, s, aspect_ratio);
            std::vector<Vector2> polyline = {p0_prime, p1_prime, p2_prime};
            Real total_area = bary_coeff(p0_cam, p1_cam, p2_cam, 1);

            //Bounding Box
            // double x_min = std::min({p0_prime.x, p1_prime.x, p2_prime.x});
            // double y_min = std::min({p0_prime.y, p1_prime.y, p2_prime.y});
            // double x_max = std::max({p0_prime.x, p1_prime.x, p2_prime.x});
            // double y_max = std::max({p0_prime.y, p1_prime.y, p2_prime.y});

            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Real pt_x = x;
                    Real pt_y = img.height - y;

                    int b = 0;
                    for (Real k = 0.125; k < 1; k+=Real(0.25)){
                        int a = 0;
                        for (Real j = 0.125; j < 1; j+=Real(0.25)){
                            Vector2 pt = Vector2{pt_x + j, pt_y + k};

                            if (rayline_intersect(pt, polyline)){
                                Vector3 camProj = screenToProj(pt, img, s, aspect_ratio);
                                
                                double b0_prime = bary_coeff(camProj, p1_cam, p2_cam, total_area);
                                double b1_prime = bary_coeff(p0_cam, camProj, p2_cam, total_area);
                                double b2_prime = bary_coeff(p0_cam, p1_cam, camProj, total_area);

                                double d = (b0_prime/t0.z) + (b1_prime/t1.z) + (b2_prime/t2.z);
                                double b0 = (b0_prime/t0.z) / d;
                                double b1 = (b1_prime/t1.z) / d;
                                double b2 = (b2_prime/t2.z) / d;

                                double p_z = (b0 * t0.z) + (b1 * t1.z) + (b2 * t2.z);

                                double n = (4 * x) + a;
                                double m = (4 * y) + b;


                                if (abs(p_z) < z_buffer(n,m)){
                                    z_buffer(n,m) = abs(p_z);
                                    large_img(n,m) = (b0 * mesh.vertex_colors[mesh.faces[i][0]]) + (b1 * mesh.vertex_colors[mesh.faces[i][1]]) + (b2 * mesh.vertex_colors[mesh.faces[i][2]]);
                                }
                            }
                            a++;
                        }
                        b++;
                    }
                }
            }
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 color = bg_color;
                    double n = 4 * x;
                    double m = 4 * y;
                    for (int i = 0; i < 4; i++){
                        for (int j = 0; j < 4; j++){
                            color += large_img(n + i, m + j);
                        }
                    }
                    img(x, y) = color / Real(16);
                }
            }
        }   
    }
    
    return img;
}

