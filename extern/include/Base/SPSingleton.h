#ifndef S_SPSINGLETON_H
#define S_SPSINGLETON_H

#pragma warning (push)
#pragma warning ( disable: 4661)

template <typename T> 
class SPSingleton
{
private:
    SPSingleton(const SPSingleton<T>&);
    SPSingleton& operator=(const SPSingleton<T>&);

protected:
    static T* Instance_;
public:
    SPSingleton(void)  { Instance_ = static_cast<T*>(this); }
    ~SPSingleton(void)  { Instance_ = 0; }
    static T* Get(void)  { return Instance_; }
};


#pragma warning (pop)

#endif
