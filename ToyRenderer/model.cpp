#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() ,uvs_(),diffusemap_(){
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

