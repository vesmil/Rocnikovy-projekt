#ifndef VALUE_H
#define VALUE_H

#include <QtConcurrent/QtConcurrent>
#include <QString>

#include <bits/unique_ptr.h>

#include "global/logcategories.h"
#include "library/controller/dependency.h"

class IValue
{
public:
    virtual void set() = 0;
    virtual void setDefault() = 0;

    virtual void store() = 0;
    virtual void restorePrev() = 0;

    virtual QString getQString() = 0;

    virtual void operator++() = 0;
    virtual void operator--() = 0;

    virtual bool isHidden() = 0;
};

template<typename TValue, typename TParam, typename TContext>
class Value : public IValue
{\
public:
    Value(TValue defaultValue, TValue minValue, TValue maxValue, bool (TContext::*setFunc) (TParam), TContext* context, QString units = "")
        : m_value(defaultValue),
          m_units(units),
          m_default(m_value),
          m_min(minValue),
          m_max(maxValue),
          m_context(context),
          m_setFunc(setFunc)
    {
    }

    TValue& getValue()
    {
        return m_value;
    }

    void set() override
    {
        // TODO lock m_value and m_prev value

        QFuture<void> future = QtConcurrent::run([&](){
            if (!(m_context->*m_setFunc)(static_cast<TParam>(m_value)))
            {
                m_value = m_prevValue;
                // m_context->refreshCurrentMenu();
            }
        });
    }

    void setDefault() override
    {
        m_value = m_default;
        set();
    }

    void store() override
    {
        m_prevValue = TValue(m_value);
    }

    void restorePrev() override
    {
        m_value = m_prevValue;
    }

    QString getQString() override
    {
        return QString::number(m_value) + m_units;
    }

    void operator++() override
    {
        if(m_value < m_max)
        {
            m_value = static_cast<TValue>(m_value + 1);
        }
        else
        {
            m_value = m_min;
        }
    }

    void operator--() override
    {
        if(m_value > m_min)
        {
            m_value = static_cast<TValue>(m_value - 1);
        }
        else
        {
            m_value = m_max;
        }
    }

    void addDependency(IDependency* dependency)
    {
        m_dependencies.emplace_back(dependency);
    }

    bool isHidden() override
    {
        for (auto& dependency : m_dependencies)
        {
            if (!dependency->isValid())
            {
                return true;
            }
        }

        return false;
    }

protected:
    TValue m_value;
    QString m_units;

    TValue m_default;
    TValue m_min;
    TValue m_max;

    TValue m_prevValue;

    TContext* m_context;
    bool (TContext::*m_setFunc) (TParam);

    std::vector<IDependency*> m_dependencies {};
};

template<typename TContext>
class BoolValue : public Value<bool, bool, TContext>
{
public:
    BoolValue(bool defaultVal, bool (TContext::*setFunc) (bool), TContext* context, QString trueMessage = "On", QString falseMessage = "Off")
        : Value<bool, bool, TContext>(defaultVal, false, true, setFunc, context, ""),
          m_onString(trueMessage), m_offString(falseMessage)
    {
    }

    QString getQString() override
    {
        return this->m_value ? m_onString : m_offString;
    }

    void operator++() override
    {
        this->m_value = !this->m_value;
    }

    void operator--() override
    {
        this->m_value = !this->m_value;
    }

private:
    QString m_onString;
    QString m_offString;
};

template<typename TVar, typename TContext, std::size_t TSize>
class ArrValue : public Value<std::size_t, TVar, TContext>
{
public:
    ArrValue(const std::array<std::pair<TVar, QString>, TSize> *array, bool (TContext::*setFunc) (TVar), TContext* context)
        : m_array(array), Value<std::size_t, TVar,TContext>(0U, 0U, TSize - 1, setFunc, context)
    {
    }

    QString getQString() override
    {
        return m_array->at(this->m_value).second;
    }


    const TVar& getValue()
    {
        return m_array->at(this->m_value).first;
    }

    void set() override
    {
        QFuture<void> future = QtConcurrent::run([&](){
            if (!(this->m_context->*this->m_setFunc)(m_array->at(this->m_value).first))
            {
                this->m_value = this->m_prevValue;
            }
        });
    }

private:
    const std::array<std::pair<TVar, QString>, TSize> *m_array;
};

#endif // VALUE_H
