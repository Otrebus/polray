#ifndef MODEL_H
#define MODEL_H

class Scene;
class Bytestream;

class Model
{
public:
    Model();
    virtual ~Model();

    static Model* Create(unsigned char n);

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;
private:
    friend class Scene;
    virtual void AddToScene(Scene& scene) = 0;
};

#endif