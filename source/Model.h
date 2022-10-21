/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Model.h
 * 
 * Declaration of the Model base class.
 */

#pragma once

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
