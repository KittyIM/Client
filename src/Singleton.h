#ifndef SINGLETON_H
#define SINGLETON_H

#include <QtCore/QMutex>

namespace Kitty
{
  template<typename T>
  class Singleton
  {
    public:
      ~Singleton() { }

      static T *inst()
      {
        static QMutex mutex;

        if(!m_inst) {
          mutex.lock();
          m_inst = new T();
          mutex.unlock();
        }

        return m_inst;
      }

      static void destr()
      {
        static QMutex mutex;

        if(m_inst) {
          mutex.lock();
          delete m_inst;
          m_inst = 0;
          mutex.unlock();
        }
      }

    protected:
      Singleton() { }
      Singleton(const T &) { }
      void operator=(const T &) { }

    protected:
      static T *m_inst;
  };

  template <typename T> T* Singleton<T>::m_inst = 0;
}

#endif // SINGLETON_H
