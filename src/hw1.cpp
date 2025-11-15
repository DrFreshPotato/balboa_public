#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{0.5, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, img.height - y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }
    // silence warnings, feel free to remove it
    UNUSED(radius);
    UNUSED(color);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 centerOfPixel = Vector2{x + 0.5, y + 0.5};
            Vector2 distToCenter = center - centerOfPixel;
            if (length(distToCenter) <= radius){
                img(x,y) = color;
            }
            else{
                img(x,y) = Vector3(0.5,0.5,0.5);
            }
        }
    }
    return img;
}

bool rayline_intersect(Vector2 pt, std::vector<Vector2> polyline){
    int count = 0;
    for (int i = 0; i < polyline.size(); i++){
        Vector2 p0 = polyline[i];
        Vector2 p1 = polyline[(i+1)%polyline.size()];

        Real t = (pt.y - p0.y) / (p1.y - p0.y);
        Real s = p0.x + (t * (p1.x - p0.x));

        if (s >= pt.x && t >= 0 && t <= 1)
        {
            if (p0.y < p1.y)
                count += 1;
            if (p0.y > p1.y)
                count -= 1;
        }
    }
    return count != 0;
}

bool point_in_line(Vector2 pt, std::vector<Vector2> polyline, bool is_closed, Real stroke_width){
    for(int i = 0; i < polyline.size(); i++){
        Vector2 p0 = polyline[i];
        Vector2 p1 = polyline[(i+1)%polyline.size()];
        
        Real l = dot((p1-p0), (pt-p0));
        l /= length(p1-p0);
        Vector2 q = p0 + l * ((p1-p0)/length(p1-p0));
        // Disables last line closing
        if (!is_closed && i+1 >= polyline.size()){
            return false;
        }
        // stroke_width = 1/2 = 0.5
        if (l > 0 && l < length(p1-p0) && length(pt-q) < stroke_width/2){
            return true;
        }
    }
    return false;
}

bool joint_line(Vector2 pt, std::vector<Vector2> polyline, bool is_closed, Real stroke_width){
    for(int i = 0; i < polyline.size(); i++){
        Vector2 p1 = polyline[(i+1)%polyline.size()];
        Vector2 centerOfJoint = p1;
        Vector2 distToCenter = centerOfJoint - pt;
        
        if (length(distToCenter) <= stroke_width / 2)
        {
            if (!is_closed && i + 2 >= polyline.size())
                return false;
            return true;
        }
    }
    return false;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render polylines
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Image3 img(640 /* width */, 480 /* height */);
    std::vector<Vector2> polyline;
    // is_closed = true indicates that the last point and
    // the first point of the polyline are connected
    bool is_closed = false;
    std::optional<Vector3> fill_color;
    std::optional<Vector3> stroke_color;
    Real stroke_width = 1;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-points") {
            while (params.size() > i+1 &&
                    params[i+1].length() > 0 &&
                    params[i+1][0] != '-') {
                Real x = std::stof(params[++i]);
                Real y = std::stof(params[++i]);
                polyline.push_back(Vector2{x, y});
            }
        } else if (params[i] == "--closed") {
            is_closed = true;
        } else if (params[i] == "-fill_color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            fill_color = Vector3{r, g, b};
        } else if (params[i] == "-stroke_color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            stroke_color = Vector3{r, g, b};
        } else if (params[i] == "-stroke_width") {
            stroke_width = std::stof(params[++i]);
        }
    }
    // silence warnings, feel free to remove it
    UNUSED(stroke_width);

    if (fill_color && !is_closed) {
        std::cout << "Error: can't have a non-closed shape with fill color." << std::endl;
        return Image3(0, 0);
    }
    Vector3 defaultbg = {0.5,0.5,0.5};
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 pt = Vector2{x+0.5,img.height - y +0.5};

            if (point_in_line(pt, polyline, is_closed, stroke_width)){
                img(x,y) = defaultbg;
                if(stroke_color.has_value()){
                    img(x,y) = stroke_color.value();
                }
            }
            else if(joint_line(pt, polyline, is_closed, stroke_width)){
                img(x,y) = defaultbg;
                if(stroke_color.has_value()){
                    img(x,y) = stroke_color.value();
                }
            }
            else if(rayline_intersect(pt, polyline)){
                img(x,y) = defaultbg;
                if(fill_color.has_value()){
                    img(x,y) = fill_color.value();
                }
            }
            else{
                img(x, y) = defaultbg;
            }
        }
    }
    return img;
}

Vector3 polyline_pixel(Vector2 pt, Polyline pl, Vector3 bg_color){
    if (point_in_line(pt, pl.points, pl.is_closed, pl.stroke_width)){
        if(pl.stroke_color.has_value()){
            return pl.stroke_color.value();
        }
    }
    if(joint_line(pt, pl.points, pl.is_closed, pl.stroke_width)){
        if(pl.stroke_color.has_value()){
            return pl.stroke_color.value();
        }
    }
    if(rayline_intersect(pt, pl.points)){
        if(pl.fill_color.has_value()){
            return pl.fill_color.value();
        }
    }
    return bg_color;
}

Vector3 circle_pixel(Vector2 pt, Circle c, Vector3 bg_color){
    Vector2 distToCenter = c.center - pt;
    if (c.stroke_color.has_value() 
        && length(distToCenter) <= c.radius + c.stroke_width 
        && length(distToCenter) > c.radius){
        return c.stroke_color.value();
    }
    else if (c.fill_color.has_value() && length(distToCenter) <= c.radius){
        if(c.fill_color.has_value()){
            return c.fill_color.value();
        }
    }

    return bg_color;
}
Image3 hw_1_3(const std::vector<std::string> &params)
{
    // Homework 1.3: render multiple shapes
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg_color = scene.background;

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector2 pt = Vector2{x + 0.5, img.height - y + 0.5};
            img(x, y) = bg_color;

            for (int i = scene.shapes.size() - 1; i >= 0; i--)
            {
                if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                {
                    Vector3 color = circle_pixel(pt, *circle, bg_color);

                    if (length(color-bg_color) != 0)
                        img(x, y) = color;
                }else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i])) {
                    Vector3 color = polyline_pixel(pt, *polyline, bg_color);
                    
                    if(length(color-bg_color) != 0){
                        img(x,y) = color;   
                    }
                }
            }
        }
    }

    return img;
}


Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg_color = scene.background;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 pt = Vector2{x + 0.5, img.height - y + 0.5};
            img(x, y) = bg_color;

            for (int i = scene.shapes.size() - 1; i >= 0; i--)
            {
                if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                {
                    Vector3 converted_pt = inverse(circle->transform) * Vector3{pt.x, pt.y, Real(1)};
                    Vector3 color = circle_pixel(Vector2{converted_pt.x, converted_pt.y}, *circle, bg_color);

                    if (length(color-bg_color) != 0)
                        img(x, y) = color;
                }else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i])) {
                    Vector3 converted_pt = inverse(polyline->transform) * Vector3{pt.x, pt.y, Real(1)};
                    Vector3 color = polyline_pixel(Vector2{converted_pt.x, converted_pt.y}, *polyline, bg_color);
                    
                    if(length(color-bg_color) != 0){
                        img(x,y) = color;   
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg_color = scene.background;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 avg = bg_color;
            for (Real k = 0.125; k < 1; k+=Real(0.25)){
                for (Real j = 0.125; j < 1; j+=Real(0.25)){
                    Real pt_x = x + j;
                    Real pt_y = img.height - y + k;

                    Vector2 pt = Vector2{pt_x, pt_y};
                    bool no_shape = true;
                    for (int i = 0; i < scene.shapes.size(); i++)
                    {
                        if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                        {
                            Vector3 converted_pt = inverse(circle->transform) * Vector3{pt.x, pt.y, Real(1)};
                            Vector3 color = circle_pixel(Vector2{converted_pt.x, converted_pt.y}, *circle, bg_color);

                            if (length(color-bg_color) != 0){
                                avg += color;
                                no_shape = false;
                                break;
                            }
                        }else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i])) {
                            Vector3 converted_pt = inverse(polyline->transform) * Vector3{pt.x, pt.y, Real(1)};
                            Vector3 color = polyline_pixel(Vector2{converted_pt.x, converted_pt.y}, *polyline, bg_color);

                            if(length(color-bg_color) != 0){
                                avg += color;  
                                no_shape = false; 
                                break;
                            }
                        }
                    }
                    if (no_shape){
                        avg += bg_color;
                    }
                }
            }
            img(x,y) = avg/Real(16);
        }
    }
    return img;
}


Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg_color = scene.background;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            bg_color = scene.background;
            Vector3 avg = bg_color;
            for (Real k = 0.125; k < 1; k+=Real(0.25)){
                for (Real j = 0.125; j < 1; j+=Real(0.25)){
                    Real pt_x = x + j;
                    Real pt_y = img.height - y + k;

                    Vector2 pt = Vector2{pt_x, pt_y};
                    bool no_shape = true;
                    Vector3 alpha_color = bg_color;
                    Vector3 shape_color = bg_color;
                    for (int i = scene.shapes.size() - 1; i >= 0; i--)
                    {
                        if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                        {
                            Vector3 converted_pt = inverse(circle->transform) * Vector3{pt.x, pt.y, Real(1)};
                            Vector3 color = circle_pixel(Vector2{converted_pt.x, converted_pt.y}, *circle, bg_color);
                            
                            if (length(color - *circle->fill_color) == 0){
                                color = (circle->fill_alpha * color) + (1-circle->fill_alpha) * alpha_color;
                            }else if (length(color - *circle->stroke_color) == 0)
                            {
                                color = (circle->stroke_alpha * color) + (1-circle->stroke_alpha) * alpha_color;
                            }
                            
                            alpha_color = color;   

                            if (length(color-bg_color) != 0){
                                shape_color = color;
                                no_shape = false;
                            }
                        }else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i])) {
                            Vector3 converted_pt = inverse(polyline->transform) * Vector3{pt.x, pt.y, Real(1)};
                            Vector3 color = polyline_pixel(Vector2{converted_pt.x, converted_pt.y}, *polyline, bg_color);
                            
                            if (length(color - *polyline->fill_color) == 0){
                                color = (polyline->fill_alpha * color) + (1-polyline->fill_alpha) * alpha_color;
                            }else if (length(color - *polyline->stroke_color) == 0)
                            {
                                color = (polyline->stroke_alpha * color) + (1-polyline->stroke_alpha) * alpha_color;
                            }
                            
                            alpha_color = color; 

                            if(length(color-bg_color) != 0){
                                shape_color = color;  
                                no_shape = false; 
                            }
                        }
                    }
                    if (no_shape){
                        shape_color = bg_color;
                    }
                    avg += shape_color;
                }
            }
            img(x,y) = avg/Real(16);
        }
    }
    return img;
}
