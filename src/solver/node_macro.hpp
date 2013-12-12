#ifndef NODE_MACRO_H
#define NODE_MACRO_H

// To save text, we use the following macro to define classes in bulk.
#define NODE(NAME, TYPE) \
class NAME : public TYPE \
{ \
public: \
    NAME(); \
    virtual ~NAME() {/*Nothing to do here*/}; \
    void eval(const float X, \
              const float Y, \
              const float Z); \
    void eval(const FabInterval& X, \
              const FabInterval& Y, \
              const FabInterval& Z); \
}

#define TRANSLATOR(NAME, TYPE) \
class NAME : public TYPE \
{ \
public: \
    NAME(); \
    NAME(Node* c); \
    virtual ~NAME() {/*Nothing to do here*/}; \
    void eval(const float X, \
              const float Y, \
              const float Z); \
    void eval(const FabInterval& X, \
              const FabInterval& Y, \
              const FabInterval& Z); \
}

#endif