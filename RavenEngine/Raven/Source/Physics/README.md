# Physics Module readme
A small demo of how to create simple collision bodies and add collision shapes to them.

## Create a collision body and a box collider
```cpp
// instantiate the physics world (a pointer)
world = physicsCommon.createPhysicsWorld();

// create a collision body at the origin
CollisionBody* bodyA = new CollisionBody(world, Transform::Identity());

// declare a box collider 
BoxCollider* colliderA = new BoxCollider();
// set the body the collider should belong to. The CollisionBody class acts a wrapper for the
// rp3d library's collision body, which is stored as a pointer in our class
colliderA->SetBody(bodyA->body);
// set our extents
colliderA->SetExtents(rp3d::Vector3(10.0f, 20.0f, 5.0f));
// create the collider shape with physics common
colliderA->CreateCollider(ColliderShapeFactory::CreateBoxShape(&physicsCommon, colliderA));

// assign the collider to the body
bodyA->AddCollider(colliderA);
```

## Serialize a collision body
### Saving:
```cpp
{
    std::ofstream os("firstSave.json");
    cereal::JSONOutputArchive oArchive(os);
    oArchive(*bodyA);
}
```
Output in Json:
```json
"value0": {
    "Colliders": [ // a vector of std::unique_ptrs
        {
            "polymorphic_id": 2147483649,
            "polymorphic_name": "Raven::BoxCollider",
            "ptr_wrapper": {
                "valid": 1,
                "data": {
                    // our saved box data
                    "Half extents": {
                        "value0": 10.0,
                        "value1": 20.0,
                        "value2": 5.0
                    }
                }
            }
        }
    ]
}
```
### Loading:
Moving on from the previous example, we delete body A and create a new body B, loading body A's data into it.
```cpp
// remove the previous body  
delete bodyA;

// create a collision body two units above the origin
Transform t;
t.SetLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
CollisionBody* bodyB = new CollisionBody(world, t);
{
    // load in collider shape data from file
    std::ifstream is("firstSave.json");
    cereal::JSONInputArchive iArchive(is);
    iArchive(*bodyB);
    // we have loaded collider data, but they neeed to be initialised in the rp3d library memory
    for (auto& c : bodyB->colliders)
    {
        c->SetBody(bodyB->body);
        // collider factory takes raw pointer to a collision shape from the unique pointer
        // (This is the only time this is done, great care must be taken when manipulating raw pointers!)
        c->CreateCollider(ColliderShapeFactory::CreateCollisionShape(&physicsCommon, c.get()));
    }
}
```
We can add more collision shapes to the body like seen above.
```cpp
// declare a box collider 
CapsuleCollider* colliderC = new CapsuleCollider();
// set the body the collider should belong to
colliderC->SetBody(bodyB->body);
// create the collider shape with physics common
colliderC->CreateCollider(ColliderShapeFactory::CreateCapsuleShape(&physicsCommon, colliderC));

// assign the collider to the body
bodyB->AddCollider(colliderC);
```
Saving to Json like above...
```
{
    std::ofstream os("secondSave.json");
    cereal::JSONOutputArchive oArchive(os);
    oArchive(*bodyB);
}
```
... gives us the following output:
```json
"value0": {
    "Colliders": [
        {
            "polymorphic_id": 2147483649,
            "polymorphic_name": "Raven::BoxCollider",
            "ptr_wrapper": {
                "valid": 1,
                "data": {
                    "Half extents": {
                        "value0": 10.0,
                        "value1": 20.0,
                        "value2": 5.0
                    }
                }
            }
        },
        {
            "polymorphic_id": 2147483650,
            "polymorphic_name": "Raven::CapsuleCollider",
            "ptr_wrapper": {
                "valid": 1,
                "data": {
                    "Radius": 1.0,
                    "Height": 1.0
                }
            }
        }
    ]
}
```
