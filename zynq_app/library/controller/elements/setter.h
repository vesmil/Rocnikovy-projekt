#ifndef SETTER_H
#define SETTER_H

#include <QString>
#include <array>
#include <vector>
#include <type_traits>

#include "library/controller/elements/controllercommand.h"
#include "library/controller/elements/iterable.h"

//! \brief Interface for a displayable value
class ISetter
{
public:
    virtual void setAsync() = 0;
    virtual void set() = 0;

    virtual void store() = 0;
    virtual void restorePrev() = 0;
    virtual void setDefault() = 0;

    virtual bool hasChanged() = 0;
    virtual bool isBinary() = 0;

    virtual QString getQString() = 0;

    virtual void increase() = 0;
    virtual void decrease() = 0;
    virtual bool isHidden() = 0;
};

/*!
 * \brief Class that represents a value (in menu) and after change it calls a function
 * @tparam TValue Type of value
 * @tparam TParam Type to which TValue is converted before calling confirmation function
 * @tparam TContext Context in which confirmation function is called
 */
template<typename TValue, typename TContext>
class Setter : public ISetter
{
public:    
    Setter(std::unique_ptr<Iterable<TValue>> iterator, bool (TContext::*setFunc) (TValue), TContext* context, QString units = "")
        : m_iterator(std::move(iterator)), m_units(units), m_context(context), m_setFunc(setFunc)
    {

    }

    void setAsync() override
    {
        m_context->addCommandToQueue(makeCommand(m_setFunc, m_context, getValue()));
    }

    void set() override
    {
        (m_context->*m_setFunc)(getValue());
    }

    void setDefault() override
    {
        m_iterator->setDefault();
        setAsync();
    }

    void restorePrev() override
    {
        m_iterator->setMarked();
    }

    bool hasChanged() override
    {
        return m_iterator->diffFromMarked();
    }

    void store() override
    {
        m_iterator->mark();
    }

    QString getQString() override
    {
        return m_iterator->text() + m_units;
    }

    void increase() override
    {
        m_iterator->next();
    }

    void decrease() override
    {
        m_iterator->prev();
    }

    bool isBinary() override
    {
        return m_iterator->isBinary();
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

    template<typename TCont, typename TVal, TVal ...TPossible>
    void addDependency(Setter<TVal, TCont> *value)
    {
       m_dependencies.emplace_back(std::make_unique<Dependency<TCont, TVal, TPossible...>>(value));
    }

private:
    virtual TValue getValue()
    {
        return m_iterator->get();
    }

    std::unique_ptr<Iterable<TValue>> m_iterator;

    QString m_units;
    TContext* m_context;

    bool (TContext::*m_setFunc) (TValue);

    //! \brief Interface so I can store dependencies in a vector
    class IDependency {
    public:
        virtual bool isValid() = 0;
    };

    //! \brief Class used to check whether dependency is valid - \a TValue is equal to some of \a TPossible
    template<typename TCont, typename TVal, TVal ...TPossible>
    class Dependency : public IDependency
    {
    public:
        explicit Dependency(Setter<TVal, TCont>* value) : m_value(value) { }

        bool isValid() override
        {
            for (auto&& possibility : {TPossible...})
            {
                if (m_value->getValue() == possibility)
                {
                    return true;
                }
            }
            return false;
        }

    private:
        Setter<TVal, TCont>* m_value;
    };

    std::vector<std::unique_ptr<IDependency>> m_dependencies {};
};

template<typename TContext>
static auto makeBoolValue(bool defaultValue, bool (TContext::*func) (bool), TContext* context, QString trueText = "On", QString falseText = "Off")
{
    return std::make_unique<Setter<bool, TContext>>( std::make_unique<BoolIterable>(defaultValue, trueText, falseText), func, context);
}

template<typename TVal, typename TContext>
static auto makeIntValue(TVal defaultValue, TVal minValue, TVal maxValue, bool (TContext::*func) (TVal), TContext* context, QString units="")
{
    return std::make_unique<Setter<TVal, TContext>>(std::make_unique<ValueIterable<TVal>>(defaultValue, minValue, maxValue), func, context, units);
}

template<typename TElem, size_t TSize, typename TContext>
static auto makeArrValue(const std::array<std::pair<TElem, QString>, TSize>& array, bool (TContext::*func) (TElem), TContext* context)
{
    PairArrayIterable<TElem, TSize> arr(array);
    return std::make_unique<Setter<TElem, TContext>>(std::make_unique<PairArrayIterable<TElem, TSize>>(array), func, context);
}

#endif // SETTER_H
