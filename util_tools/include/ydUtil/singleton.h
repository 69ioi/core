#ifndef __COMMON_SINGLETON_H_FILE__
#define __COMMON_SINGLETON_H_FILE__

template <class T>
class Singleton
{
    protected:
        Singleton(){
        };
        virtual ~Singleton(){

        };
    public:
        static T *Instance( bool destory = false )
        {
            static T *s_instance = 0;
            if ( destory )
            {
                if ( s_instance )
                {
                    delete s_instance;
                    s_instance = 0;
                }
                return 0;
            }
            if ( !s_instance )
            {
                s_instance = new T;
            }
            return s_instance;
        }

        static void Destory()
        {
            Instance( true );
        }

};


#endif // __COMMON_SINGLETON_H_FILE__
