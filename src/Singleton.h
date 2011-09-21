#ifndef SINGLETON_H
#define SINGLETON_H

#include <QtCore/QPointer>
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
          mutex.unlock();
        }
      }

    protected:
      Singleton() { }
      Singleton(const T &) { }
      void operator=(const T &) { }

    protected:
      static QPointer<T> m_inst;
  };

  template <typename T> QPointer<T> Singleton<T>::m_inst = 0;
}

#endif // SINGLETON_H
