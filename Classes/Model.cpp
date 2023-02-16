//
// Created by pedrovincius on 20/03/2022.
//
#include <vector>
#include <iostream>
#include <fstream>
#include <GL/gl.h>
#include <sstream>

class Model {
public:

    explicit Model(const std::string& file_name) {
        std::ifstream file(file_name);
        if (!file.is_open()) {
            std::cerr << "Não foi possível abrir o arquivo: " << file_name << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if (prefix == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                vertices_.push_back({x, y, z});
            } else if (prefix == "vn") {
                float x, y, z;
                iss >> x >> y >> z;
                normals_.push_back({x, y, z});
            } else if (prefix == "f") {
                std::string current_line = line;
                std::istringstream iss2(current_line.replace(0, 2, ""));

                std::string face_vertex;
                std::vector<int> vertices, normals;

                while (iss2 >> face_vertex) {
                    int vertex, texture_coord, normal;
                    sscanf(face_vertex.c_str(), "%d/%d/%d", &vertex, &texture_coord, &normal);
                    vertices.push_back(vertex);
                    normals.push_back(normal);
                }

                faces_.emplace_back(vertices, normals);
            }
        }
        long unsigned int i = vertices_.size();
        long unsigned int j = faces_.size();
        long unsigned int k = normals_.size();

        file.close();
    }

    void draw(){
        for (const auto& face : faces_) {
            const auto& vertex_indices = face.first;
            const auto& normal_indices = face.second;
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < 3; ++i) {
                const auto& vertex = vertices_[vertex_indices[i] - 1];
                const auto& normal = normals_[normal_indices[i] - 1];
                glNormal3f(normal[0], normal[1], normal[2]);
                glVertex3f(vertex[0], vertex[1], vertex[2]);
            }
            glEnd();
        }
    }

private:
    std::vector<std::vector<float>> vertices_;
    std::vector<std::vector<float>> normals_;
    std::vector<std::pair<std::vector<int>, std::vector<int>>> faces_;
};
