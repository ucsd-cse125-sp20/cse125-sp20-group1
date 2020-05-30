#include "GameManager.hpp"

using namespace std;
namespace pt = boost::property_tree;

GameManager::GameManager(): updateTerrain(false){
    currTime = "";
    //startTime = clock();
    startTime = time(NULL);
    totalGameTime = 100.0f;
    terrain = new Terrain(251, 251, 0.5f);
    std::vector<glm::vec2> tmp = {
        glm::vec2(1.0f, 1.0f),
        glm::vec2(125.0f, 125.0f),
        glm::vec2(135.0f, 125.0f),
        glm::vec2(250.0f, 250.0f)
    };
    terrain->edit(tmp, 0);
    terrain->computeBoundingBoxes();

    sphere1 = new Sphere(5.0f, 2.0f);
    sphere1->move(glm::vec3(64,2,-65));

    sphere2 = new Sphere(5.0f, 2.0f);
    sphere2->move(glm::vec3(58,2,-54));

    for (int i = 0; i < 4 ; i++){
        mutex_arr[i].lock();
        edited_terrains.push_back({});
        mutex_arr[i].unlock();
    }
}

void GameManager::UpdateScore(){
    //obj.score++;
    // Need to determine which team to add score
}

int GameManager::UpdateTime(){
    string finishedTime = "";
    endTime = time(NULL);
    //timeSignal = 1;
    //float duration = totalGameTime - (float)(endTime-startTime) / CLOCKS_PER_SEC;
    double duration = totalGameTime - difftime(endTime, startTime);
    finishedTime = finishedTime + to_string((int)duration/60);
    if((int)duration % 60 < 10){
        finishedTime += ":0" + to_string((int)duration%60);
    } else {
        finishedTime += ":" + to_string((int)duration%60);
    }
    if(duration <= 0){
        // Send a signal to announce game ends
        duration = 0;
        //timeSignal = 0;
        finishedTime = "0:00";
        return 0;
    }
    //cout << finishedTime << endl;
    currTime = finishedTime;
    return 1;
}

void GameManager::update1(char op, glm::vec3 lookat){
    glm::vec3 newPos;
    glm::vec3 right = glm::normalize(glm::cross(lookat, glm::vec3(0.0f, 1.0f, 0.0f)));
    lookat = glm::normalize(lookat);

    float speed = 20.0f;

    switch (op) {
        case 'w':{
            glm::vec3 f = sphere1->moveForce;
            f.x += lookat.x * speed;
            f.z += lookat.z * speed;
            sphere1->moveForce = f;
            break;
        }
        case 'a':{
            glm::vec3 f = sphere1->moveForce;
            f.x -= right.x * speed;
            f.z -= right.z * speed;
            sphere1->moveForce = f;
            break;
        }
        case 's':{
            glm::vec3 f = sphere1->moveForce;
            f.x -= lookat.x * speed;
            f.z -= lookat.z * speed;
            sphere1->moveForce = f;
            break;
        }
        case 'd':{
            glm::vec3 f = sphere1->moveForce;
            f.x += right.x * speed;
            f.z += right.z * speed;
            sphere1->moveForce = f;
            break;
        }
    }
}

void GameManager::update2(char op, glm::vec3 lookat){
    glm::vec3 newPos;
    glm::vec3 right = glm::normalize(glm::cross(lookat, glm::vec3(0.0f, 1.0f, 0.0f)));

    lookat = glm::normalize(lookat);

    float speed = 20.0f;

    switch (op) {
        case 'w':{
            glm::vec3 f = sphere2->moveForce;
            f.x += lookat.x * speed;
            f.z += lookat.z * speed;
            sphere2->moveForce = f;
            break;
        }
        case 'a':{
            glm::vec3 f = sphere2->moveForce;
            f.x -= right.x * speed;
            f.z -= right.z * speed;
            sphere2->moveForce = f;
            break;
        }
        case 's':{
            glm::vec3 f = sphere2->moveForce;
            f.x -= lookat.x * speed;
            f.z -= lookat.z * speed;
            sphere2->moveForce = f;
            break;
        }
        case 'd':{
            glm::vec3 f = sphere2->moveForce;
            f.x += right.x * speed;
            f.z += right.z * speed;
            sphere2->moveForce = f;
            break;
        }
    }
}

void GameManager::editTerrain(std::vector<glm::vec2> & editPoints, float height){
    //cout << "editing terrain..." << endl;
    //std::cout << editPoints[0][0] << ", " << editPoints[0][1] << " & " << editPoints[1][0] << ", " << editPoints[1][1] << std::endl;
    glm::vec2 sT = glm::vec2(editPoints[0][0] * 2, editPoints[0][1] * -2);
    glm::vec2 eT = glm::vec2(editPoints[1][0] * 2, editPoints[1][1] * -2);
    std::vector<glm::vec2> temp = {sT, eT};

    edited_points.push_back(std::to_string(sT[0]) + "," + std::to_string(sT[1]) + ","
        + std::to_string(eT[0]) + "," + std::to_string(eT[1]) + "," + std::to_string(height));
    //terrain->edit(temp, height);
}

void GameManager::handle_input(string data, int id){
    //cout << data << endl;
    std::string key_op = "";
    std::string mouse_op = "";

    std::vector<glm::vec2> editPoints;
    float height = 10;
    glm::vec3 camLookatFront = glm::vec3(0.0);
    decode(id, data, key_op, mouse_op, camLookatFront, editPoints);

    //cout << camLookatFront.x << " " << camLookatFront.y << " " << camLookatFront.z << " " << endl;

    if(key_op != ""){
        cout << "id: " << id << ", operation: "<< key_op << endl;
        if(id == 1){
            update1(key_op.at(0), camLookatFront);
        }else if(id == 2){
            update2(key_op.at(0), camLookatFront);
        }
    }
    cout << id << endl;
    if(!editPoints.empty()){
        if(mouse_op.compare("l") == 0){
            glm::vec2 sT = glm::vec2(editPoints[0][0] * 2, editPoints[0][1] * -2);
            glm::vec2 eT = glm::vec2(editPoints[1][0] * 2, editPoints[1][1] * -2);
            std::vector<glm::vec2> temp = {sT, eT};
            for (int i = 0; i < 4; ++i)
            {
                edited_terrains[i].push_back(std::to_string(sT[0]) + "," + std::to_string(sT[1]) + ","
                    + std::to_string(eT[0]) + "," + std::to_string(eT[1]) + "," + std::to_string(height));
            }

        } 
        else if(mouse_op.compare("r") == 0){
            glm::vec2 sT = glm::vec2(editPoints[0][0] * 2, editPoints[0][1] * -2);
            glm::vec2 eT = glm::vec2(editPoints[1][0] * 2, editPoints[1][1] * -2);
            std::vector<glm::vec2> temp = {sT, eT};
            for(int i = 0; i < 4; i++){
                edited_terrains[i].push_back(std::to_string(sT[0]) + "," + std::to_string(sT[1]) + ","
                    + std::to_string(eT[0]) + "," + std::to_string(eT[1]) + "," + std::to_string(height * -1));
            }
        }

        updateTerrain = true;
    }
    // add gravity for now
    sphere1->force += glm::vec3(0, -9.8, 0);
    sphere2->force += glm::vec3(0, -9.8, 0);

    checkTerrainCollisions(sphere1);
    checkTerrainCollisions(sphere2);
    checkSphereCollisions();
}


string GameManager::encode(int id)
{
    transM1 = sphere1->getModel();
    transM2 = sphere2->getModel();
    pt::ptree root;
    pt::ptree obj;

    pt::ptree obj1;
    pt::ptree obj2;

    pt::ptree m1;
    pt::ptree m2;

    pt::ptree matrix1[16];
    pt::ptree matrix2[16];

    pt::ptree height_root;

    pt::ptree timeNode;
    pt::ptree scoreNode;

    obj1.put("id", 1);
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            matrix1[4*i+j].put("", transM1[i][j]);
        }
    }
    for(int i=0;i<16;i++){
        m1.push_back(std::make_pair("", matrix1[i]));
    }
    obj1.add_child("transformation", m1);

    obj2.put("id", 2);
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            matrix2[4*i+j].put("", transM2[i][j]);
        }
    }

    for(int i=0;i<16;i++){
        m2.push_back(std::make_pair("", matrix2[i]));
    }
    obj2.add_child("transformation", m2);
    obj.push_back(std::make_pair("", obj1));
    obj.push_back(std::make_pair("", obj2));
    
    //if(updateTerrain){
        // build and add current height map node to root

    while(!edited_terrains[id - 1].empty()){
        pt::ptree node;
        mutex_arr[id - 1].lock();
        string res = edited_terrains[id - 1].back();
        edited_terrains[id - 1].pop_back();
        mutex_arr[id - 1].unlock();
        node.put("", res);
        height_root.push_back(std::make_pair("", node));
    }

    pt::ptree tempNodeS1;
    pt::ptree tempNodeS2;
    tempNodeS1.put("", scoreT1);
    tempNodeS2.put("", scoreT2);
    scoreNode.push_back(std::make_pair("", tempNodeS1));
    scoreNode.push_back(std::make_pair("", tempNodeS2));
    // Add time to root
    pt::ptree tempNodeT;

    string finishedTime = "";
    endTime = time(NULL);
    //timeSignal = 1;
    //float duration = totalGameTime - (float)(endTime-startTime) / CLOCKS_PER_SEC;
    double duration = totalGameTime - difftime(endTime, startTime); 
    finishedTime = finishedTime + to_string((int)duration/60);
    if((int)duration % 60 < 10){
        finishedTime += ":0" + to_string((int)duration%60);
    } else {
        finishedTime += ":" + to_string((int)duration%60);
    }
    //cout << finishedTime << endl;


    tempNodeT.put("", finishedTime);
    timeNode.push_back(std::make_pair("",tempNodeT));

    root.add_child("Obj", obj);
    
    root.add_child("edited_points" ,height_root);
    
    root.add_child("Score", scoreNode);

    root.add_child("Time", timeNode);

    root.put("Header", "update");

    stringstream ss;
    write_json(ss, root, false);
    return ss.str() + '\n';
}

void GameManager::checkTerrainCollisions(Sphere* sphere) {
    std::vector<unsigned int>* indices = terrain->getIndices();
    std::vector<glm::vec3>* vertices = terrain->getVertices();
    std::vector<TerrainBoundingBox>* boxes = terrain->getBoundingBoxes();

    float elapsedTime = 0.03f / 20;
    for (int k = 0; k < 20; k++) {
        for (int j = 0; j < boxes->size(); j++) {
            TerrainBoundingBox& box = (*boxes)[j];
            glm::vec2& tminPoint = box.minPoint;
            glm::vec2& tmaxPoint = box.maxPoint;
            glm::vec2 sminPoint(sphere->getCenter().x, sphere->getCenter().z);
            sminPoint += glm::vec2(-sphere->getRadius(), -sphere->getRadius());
            glm::vec2 smaxPoint(sphere->getCenter().x, sphere->getCenter().z);
            smaxPoint += glm::vec2(sphere->getRadius(), sphere->getRadius());

            if (sminPoint.x > tmaxPoint.x || tminPoint.x > smaxPoint.x || sminPoint.y > tmaxPoint.y || tminPoint.y > smaxPoint.y) { // not in box
                continue;
            }

            for (int i = 0; i < box.indices2triangles.size(); i++) {
                int curInd = box.indices2triangles[i];
                glm::vec3& a = (*vertices)[(*indices)[curInd-2]];
                glm::vec3& b = (*vertices)[(*indices)[curInd-1]];
                glm::vec3& c = (*vertices)[(*indices)[curInd]];
                glm::vec3 n = -glm::normalize(glm::cross(c-a, b-a));
                if (glm::dot(n, glm::vec3(0, 1, 0)) < 0) { // little hack to make sure normals are upwards
                    n = -n;
                }

                glm::vec3 offset = sphere->checkCollision(a, b, c, n);
                if (glm::length(offset) < 0.0001f) { // clamp to avoid bouncing too many times
                    offset = glm::vec3(0);
                    continue;
                }
                if (!isnan(offset.x)) {
                    //std::cout << glm::to_string(offset) << std::endl;
                }
                sphere->move(sphere->getCenter() + offset); // move to right position
            }
            sphere->momentum += sphere->force * elapsedTime;
            glm::vec3 dis = (sphere->momentum/sphere->mass) * elapsedTime;

            if (glm::length(sphere->moveMomentum) > 0) {
                glm::vec3 temp = 10.0f * glm::normalize(sphere->moveMomentum) * elapsedTime;
                if (glm::length(temp) >= glm::length(sphere->moveMomentum)) {
                    sphere->moveMomentum = glm::vec3(0);
                } else {
                    sphere->moveMomentum -= 10.0f * glm::normalize(sphere->moveMomentum) * elapsedTime;
                }
            }
            sphere->moveMomentum += sphere->moveForce * elapsedTime;
            if (glm::length(sphere->moveMomentum) > 20.0f) {
                sphere->moveMomentum = 20.0f * glm::normalize(sphere->moveMomentum);
            }
            //std::cout << glm::to_string(sphere->moveMomentum) << std::endl;
            //std::cout << sphere->mass << std::endl;
            dis += (sphere->moveMomentum/sphere->mass) * elapsedTime;
            //std::cout << glm::to_string(dis) << std::endl;

            if (!isnan(sphere->getCenter().x)) {
                //std::cout << glm::to_string(sphere->getCenter()) << std::endl;
            }
            sphere->move(sphere->getCenter() + dis);

        }
    }
    sphere->force = glm::vec3(0);
    sphere->moveForce = glm::vec3(0);

    // if sphere has fallen off, freaking lift it up
    float height = terrain->getHeightAt(sphere->getCenter().x, sphere->getCenter().z);
    //std::cout << height << std::endl;
    //std::cout << sphere->getCenter().y + sphere->getRadius() << std::endl;
    if (height > sphere->getCenter().y + sphere->getRadius()) {
        glm::vec3 offset(0);
        offset.y = height - (sphere->getCenter().y - sphere->getRadius());
        sphere->move(sphere->getCenter() + offset);
    }
}

void GameManager::decode(int id, string data, string & key_op, string & mouse_op, glm::vec3 & camLookatFront, vector<glm::vec2> & editPoints)
{
    float temp[4];
    // Read JSON from client
    try{
        if(data != ""){
            stringstream ss;
            ss << data;

            pt::ptree tar;
            pt::read_json(ss, tar);

            string header = tar.get<string>("Header");
            if(header.compare("restart") == 0){
                restartSet.insert(id);
                if(restartSet.size() == 4){
                    restartGame();
                }
            }else if(header.compare("data") == 0){
                int i = 0;
                BOOST_FOREACH(const pt::ptree::value_type& child, tar.get_child("cmd")) {
                    if(i == 0){
                        key_op = child.second.get<std::string>("key");
                    }
                    else if (i == 1){
                        // Mouse
                        mouse_op = child.second.get<std::string>("mouse");

                        if(mouse_op.compare("l") == 0){
                            int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("mouse_l")){
                                temp[index] = stof(t.second.data());
                                index++;
                            }
                        } else if(mouse_op.compare("r") == 0){
                            int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("mouse_r")){
                                temp[index] = stof(t.second.data());
                                index++;
                            }
                        }
                    }
                    else if( i == 2){
                        // Cam
                        float temp1[3];
                        int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("cam_lookatfront")){
                                temp1[index] = stof(t.second.data());
                                index++;
                            }
                        camLookatFront.x = temp1[0];
                        camLookatFront.y = temp1[1];
                        camLookatFront.z = temp1[2];
                    }
                    i++;
                }
            }

        }
    }catch(...){
        std::cout << "decode exception" << std::endl;
    }
    if(mouse_op != ""){
        editPoints.push_back(glm::vec2(temp[0],temp[1]));
        editPoints.push_back(glm::vec2(temp[2],temp[3]));
    }
}

void GameManager::restartGame(){
    cout << "restart " << endl;
    currTime = "";
    //startTime = clock();
    startTime = time(NULL);
    totalGameTime = 100.0f;

    sphere1->move(glm::vec3(64,2,-65));
    sphere2->move(glm::vec3(30,2,-20));
}
void GameManager::checkSphereCollisions() {
    glm::vec3 sphere1Pos = sphere1->getCenter();
    glm::vec3 sphere2Pos = sphere2->getCenter();
    float sphere1Radius = sphere1->getRadius();
    float sphere2Radius = sphere2->getRadius();
    float delta = glm::length(sphere1Pos - sphere2Pos);
    if (delta < sphere1Radius + sphere2Radius) {
        glm::vec3 dir = glm::normalize(sphere1Pos - sphere2Pos);
        sphere1->move(sphere1Pos + delta / 2.0f * dir);
        sphere2->move(sphere1Pos - delta / 2.0f * dir);
    }
    // TODO: further physics effect
}
