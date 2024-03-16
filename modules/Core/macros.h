#ifndef MACROS_H
#define MACROS_H

#include <QObject>

// default variant. getter + setter
#define UA_PROP_RW(type, name, setter)                                 \
private:                                                               \
    Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed)  \
public:                                                                \
    type name() const { return m_##name ; }                            \
Q_SIGNALS:                                                             \
    void name##Changed();                                              \
public Q_SLOTS:                                                        \
    void setter(type name) {                                           \
        if (m_##name == name) return;                                  \
         m_##name = name;                                              \
         emit name##Changed();                                         \
    }                                                                  \
private:                                                               \
    type m_##name;


// getter + setter but setter must be done separately
#define UA_PROP_RWS(type, name, setter)                                \
private:                                                               \
    Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed)  \
    public:                                                            \
    type name() const { return m_##name ; }                            \
    Q_SIGNALS:                                                         \
    void name##Changed();                                              \
    private:                                                           \
    type m_##name;

// proxy to another object
#define UA_PROXY_RW(type, object, name, setter)                        \
private:                                                               \
    Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed)  \
public:                                                                \
    type name() const { return object.name() ; }                       \
Q_SIGNALS:                                                             \
    void name##Changed();                                              \
public Q_SLOTS:                                                        \
    void setter(type name) { object.setter(name); }                    \
private:                                                               \
    void name##Register(){connect(&object, SIGNAL(name##Changed()),    \
                                    this, SIGNAL(name##Changed()));}

// proxy to another object. Object is pointer
#define UA_PROXY_RWP(type, object, name, setter)                       \
private:                                                               \
    Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed)  \
public:                                                                \
    type name() const { return object->name() ; }                      \
Q_SIGNALS:                                                             \
    void name##Changed();                                              \
public Q_SLOTS:                                                        \
    void setter(type name) { object->setter(name); }                   \
private:                                                               \
    void name##Register(){connect(object, SIGNAL(name##Changed()),     \
                                    this, SIGNAL(name##Changed()));}

// Read only property
#define UA_PROP_RO(type, name)                                         \
private:                                                               \
    Q_PROPERTY(type name READ name NOTIFY name##Changed)               \
public:                                                                \
    type name() const { return m_##name ; }                            \
Q_SIGNALS:                                                             \
    void name##Changed();                                              \
private:                                                               \
    type m_##name;

//define
#define UA_DEF(type, name, value)                                      \
    Q_PROPERTY(type name MEMBER name CONSTANT)                         \
    inline static const type name = value;


#endif // MACROS_H
