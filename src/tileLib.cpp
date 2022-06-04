#include "tileLib.hpp"

Map::Map(string ipath, int iTileWidth) : path(ipath) {
    cout << "Parsing " << path << " to Map" << endl;

    // Read the xml file into a vector
    ifstream xmlTilesFile(path);
    if (xmlTilesFile.fail()) {
        cout << "File " << path << "doesn't exist. This will cause errors" << endl;
    }
    vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing library into doc 

    mapNode = doc.first_node("map"); // Find our root node
    tileSetNode = mapNode->first_node("tileset");

    tileSetPath = tileSetNode->first_attribute("source")->value();

    //tileWidth = atoi(mapNode->first_attribute("tilewidth")->value());
    //tileHeight = atoi(mapNode->first_attribute("tileheight")->value());

    tileWidth = iTileWidth;
    tileHeight = iTileWidth;

    width = atoi(mapNode->first_attribute("width")->value());
    height = atoi(mapNode->first_attribute("height")->value());
    layerCount = atoi(mapNode->first_attribute("nextlayerid")->value()) - 1;

    mapWidth = tileWidth*width;
    mapHeight = tileHeight*height;

    tiles = new short[layerCount * height * width];

    //iterate through layers
    tempLayerNode = mapNode->first_node("layer"); 
    string dataDelimiter = ",";

    for (int layerID = 1; layerID <= layerCount; layerID ++) {
        tempDataNode = tempLayerNode->first_node("data");
        string layerData = tempDataNode->value();
        int pos;

        // remove all \n & ' ' from layerData
        while ((pos = layerData.find('\n'))!=string::npos) {layerData.replace(pos, 1, "");}
        while ((pos = layerData.find(' '))!=string::npos) {layerData.replace(pos, 1, "");}

        //needed to parse the last tileID
        layerData.append(",");
        
        //Parse tiles
        int layer = layerID-1;
        int index = 0;
        while ((pos = layerData.find(dataDelimiter)) != string::npos) {
            tiles[layer* width*height + index] = stoi(layerData.substr(0, pos));
            index ++;
            layerData.erase(0, pos + dataDelimiter.length());
        }
        //go to next layer
        tempLayerNode = tempLayerNode->next_sibling();
    }
}

short Map::getID(int tx, int ty, int layer) {
    return tiles[layer* width*height + ty * width + tx]; 
}

bool Map::isColliding(TileSet* tileSet,const Vector2D& pos) {
    float tempTx = ((int)pos.x)/tileWidth; //without the rest!
    float tempTy = ((int)pos.y)/tileHeight;
    short tempID = getID(tempTx,tempTy,1);
    return (0<tempID && tempID <(tileSet->tileCount));
}

void Map::printTiles() {
    for (int layer = 0; layer<layerCount; layer++) {
        cout << "Layer " << layer+1 << ":" << endl;
        for (int y = 0; y<height; y++) {
            for (int x = 0; x<width; x++) {
                cout << tiles[layer* width*height + y * width + x] << ",";
            }
            cout << endl;
        }
    }
}

void Map::printProperties() {
    cout << "Map " << path << ":" << endl;
    cout << "Tileset Path:" << tileSetPath << endl;
    cout << "Map Columns:" << width << " Rows:" << height << endl;
    cout << "Map PixelW:" << mapWidth << " H:" << mapHeight << endl;
    cout << "Number of layers: " << layerCount << endl;
    cout << "Tile W:" << tileWidth << " H:" << tileHeight << endl;
}

void Map::render(TileSet* tileSet, int layer, const Vector2D& offset) {
    for (int ty = 0; ty<height; ty++) {
        for (int tx = 0; tx<width; tx++) {
        Vector2D tempPos(tx*tileWidth+offset.x, ty*tileWidth+offset.y);
        Vector2D tempSize(tileWidth,tileWidth);
        tileSet->renderTile(getID(tx,ty,layer),tempPos,tempSize);
        }
    }
}



TileSet::TileSet(string ipath, SDL_Renderer* renderer) : path(ipath), renderer(renderer) {
    cout << "Parsing " << path << " to TileSet Class" << endl;

    // Read the xml file into a vector
    ifstream xmlTilesFile(path);
    if (xmlTilesFile.fail()) {
        cout << "File " << path << "doesn't exist. This will cause errors" << endl;
    }

    vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing library into doc 

    root_node = doc.first_node("tileset"); // Find our root node
    element_node = root_node->first_node("image");

    tileWidth = atoi(root_node->first_attribute("tilewidth")->value());
    tileHeight = atoi(root_node->first_attribute("tileheight")->value());
    tileCount = atoi(root_node->first_attribute("tilecount")->value());
    tileColumns = atoi(root_node->first_attribute("columns")->value());
    tileRows = tileCount/tileColumns;
    imagePath = string(RESOURCE_PATH) + element_node->first_attribute("source")->value();
    imagePath.insert(0,"./");

    imageWidth = atoi(element_node->first_attribute("width")->value());
    imageHeight = atoi(element_node->first_attribute("height")->value());

    const char *c = imagePath.c_str();
    sourceImageSet = IMG_Load(c);
    sourceTexSet = SDL_CreateTextureFromSurface(renderer, sourceImageSet);
    SDL_FreeSurface(sourceImageSet);
}

TileSet::~TileSet() {
    SDL_DestroyTexture(sourceTexSet);
}

void TileSet::renderTest() {
    SDL_Rect tempSourceRect = {0,0,imageWidth,imageHeight};
    SDL_Rect tempDestRect = {0,0,tileWidth,tileHeight};
    SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
}

SDL_Rect TileSet::getRectOfID(short ID) {
    SDL_Rect tempRect = {0,0,tileWidth,tileHeight};

    tempRect.x = ID%tileColumns-1;
    tempRect.y = ((ID-tempRect.x)/tileColumns);
    tempRect.x *= tileWidth;
    tempRect.y *= tileHeight;
    return tempRect;
}

void TileSet::renderTile(short ID, const Vector2D& pos, const Vector2D& size) {
    SDL_Rect tempSourceRect = getRectOfID(ID);
    SDL_Rect tempDestRect = {(int)pos.x,(int)pos.y,(int)size.x,(int)size.y};
    SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
} 

void TileSet::printProperties() {
    cout << "Tileset " << path << ":" << endl;
    cout << "Imagepath:" << imagePath << endl;
    cout << "Image W:" << imageWidth << " H:" << imageHeight << endl;
    cout << "Tile W:" << tileWidth << endl;
    cout << "Tile H:" << tileHeight << endl;
    cout << "Tile Columns:" << tileColumns << endl;
    cout << "Tile Count:" << tileCount << endl;
}
