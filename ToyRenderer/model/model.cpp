#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename, bool skybox) : verts_(), faces_() ,uvs_(),diffusemap_(){
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) { //eof = file end
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } 
        else if(!line.compare(0, 2, "vt"))
        {
            iss >> trash >>trash;
            vec2 uv;
            for (int i = 0; i < 2; i++)
            {
                iss>>uv[i];
            }
            uvs_.push_back(uv);
        }
        else if (!line.compare(0, 2, "vn"))
        {
            iss >> trash >> trash;
            vec3 normal;
            for (int i = 0; i < 3; i++)
            {
                iss >> normal[i];
            }
            normals_.push_back(normal);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<vec3> f;
            vec3 tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }

    load_texture(filename, "_diffuse.tga", diffusemap_);
    load_texture(filename,"_nm.tga",normalmap_);
    load_texture(filename, "_spec.tga", specularmap_);
    load_texture(filename, "_nm_tangent.tga", tangentNormalmap_);
    if (skybox)
    {
        load_texture(filename, "_front1.tga", cubemap_[0]);
        load_texture(filename, "_back1.tga", cubemap_[1]);
        load_texture(filename, "_top1.tga", cubemap_[2]);
        load_texture(filename, "_bottom1.tga", cubemap_[3]);
        load_texture(filename, "_left1.tga", cubemap_[4]);
        load_texture(filename, "_right1.tga", cubemap_[5]);
    }
    in.close();
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i = 0; i < (int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

vec3 Model::vert(int iface, int nvert) {
    return verts_[faces_[iface][nvert][0]];
}

vec2 Model::uv(int iface, int nvert) {
    int idx = faces_[iface][nvert][1];
    return vec2(uvs_[idx].x * diffusemap_.get_width(), uvs_[idx].y * diffusemap_.get_height());
}

vec3 Model::normal(int iface, int nvert) {
    int idx = faces_[iface][nvert][2];
    return normals_[idx].normalize();
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img) {
    std::string textfile(filename);
    size_t dot = textfile.find_last_of(".");
    if (dot != std::string::npos) {
        textfile = textfile.substr(0, dot) + std::string(suffix);
        std::cout << "textfile file" << textfile << "loading " <<
            (img.read_tga_file(textfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

TGAColor Model::diffuse(vec2 uv){
    return diffusemap_.get(uv[0],uv[1]);
}

double Model::specular(vec2 uv)
{
    return specularmap_.get(uv[0], uv[1])[0];
}

TGAColor Model::cubemap(int index, vec2 uv)
{
    uv[0] = fmod(uv[0], 1);
    uv[1] = fmod(uv[1], 1);
    float uv_1 = uv.x* cubemap_[index].get_width();
    float uv_2 = uv.y* cubemap_[index].get_height();
    return cubemap_[index].get(uv_1,uv_2);
}

vec3 Model::tangent_normal(vec2 uv){
    //纹理取值（0，1） 法线（ - 1，1） 需要变换一下 normal = color * 2 - 1
    TGAColor color = tangentNormalmap_.get(uv[0], uv[1]);
    vec3 tangent_normal(color[2]/255.*2.f-1.,color[1]/255.*2.-1.,color[0]/255.*2.-1.);
    return tangent_normal;
}

vec3 Model::normal(vec2 uv) {
    TGAColor color =  normalmap_.get(uv[0], uv[1]);
    vec3 normal;
    normal[0] = color[2] / 255. * 2 - 1;
    normal[1] = color[1] / 255. * 2 - 1;
    normal[2] = color[0] / 255. * 2 - 1;
    return normal;
}

