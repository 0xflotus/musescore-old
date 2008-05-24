#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <qgraphicssceneevent.h>
#include <QVariant>
#include <qpoint.h>
#include <qwidget.h>

static const char * const qtscript_QGraphicsSceneHoverEvent_function_names[] = {
    "QGraphicsSceneHoverEvent"
    // static
    // prototype
    , "lastPos"
    , "lastScenePos"
    , "lastScreenPos"
    , "modifiers"
    , "pos"
    , "scenePos"
    , "screenPos"
    , "setLastPos"
    , "setLastScenePos"
    , "setLastScreenPos"
    , "setModifiers"
    , "setPos"
    , "setScenePos"
    , "setScreenPos"
    , "toString"
};

static const char * const qtscript_QGraphicsSceneHoverEvent_function_signatures[] = {
    "Type type"
    // static
    // prototype
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , "QPointF pos"
    , "QPointF pos"
    , "QPoint pos"
    , "KeyboardModifiers modifiers"
    , "QPointF pos"
    , "QPointF pos"
    , "QPoint pos"
""
};

static QScriptValue qtscript_QGraphicsSceneHoverEvent_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("QFile::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QGraphicsSceneHoverEvent*)
Q_DECLARE_METATYPE(QFlags<Qt::KeyboardModifier>)
Q_DECLARE_METATYPE(QEvent::Type)
Q_DECLARE_METATYPE(QGraphicsSceneEvent*)

//
// QGraphicsSceneHoverEvent
//

static QScriptValue qtscript_QGraphicsSceneHoverEvent_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 14;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    QGraphicsSceneHoverEvent* _q_self = qscriptvalue_cast<QGraphicsSceneHoverEvent*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("QGraphicsSceneHoverEvent.%0(): this object is not a QGraphicsSceneHoverEvent")
            .arg(qtscript_QGraphicsSceneHoverEvent_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QPointF _q_result = _q_self->lastPos();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QPointF _q_result = _q_self->lastScenePos();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        QPoint _q_result = _q_self->lastScreenPos();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QFlags<Qt::KeyboardModifier> _q_result = _q_self->modifiers();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        QPointF _q_result = _q_self->pos();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QPointF _q_result = _q_self->scenePos();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        QPoint _q_result = _q_self->screenPos();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 1) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        _q_self->setLastPos(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 8:
    if (context->argumentCount() == 1) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        _q_self->setLastScenePos(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 9:
    if (context->argumentCount() == 1) {
        QPoint _q_arg0 = qscriptvalue_cast<QPoint>(context->argument(0));
        _q_self->setLastScreenPos(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 10:
    if (context->argumentCount() == 1) {
        QFlags<Qt::KeyboardModifier> _q_arg0 = qscriptvalue_cast<QFlags<Qt::KeyboardModifier> >(context->argument(0));
        _q_self->setModifiers(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 11:
    if (context->argumentCount() == 1) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        _q_self->setPos(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 12:
    if (context->argumentCount() == 1) {
        QPointF _q_arg0 = qscriptvalue_cast<QPointF>(context->argument(0));
        _q_self->setScenePos(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 13:
    if (context->argumentCount() == 1) {
        QPoint _q_arg0 = qscriptvalue_cast<QPoint>(context->argument(0));
        _q_self->setScreenPos(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 14: {
    QString result = QString::fromLatin1("QGraphicsSceneHoverEvent");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_QGraphicsSceneHoverEvent_throw_ambiguity_error_helper(context,
        qtscript_QGraphicsSceneHoverEvent_function_names[_id+1],
        qtscript_QGraphicsSceneHoverEvent_function_signatures[_id+1]);
}

static QScriptValue qtscript_QGraphicsSceneHoverEvent_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("QGraphicsSceneHoverEvent(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QGraphicsSceneHoverEvent* _q_cpp_result = new QGraphicsSceneHoverEvent();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QEvent::Type _q_arg0 = qscriptvalue_cast<QEvent::Type>(context->argument(0));
        QGraphicsSceneHoverEvent* _q_cpp_result = new QGraphicsSceneHoverEvent(_q_arg0);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_QGraphicsSceneHoverEvent_throw_ambiguity_error_helper(context,
        qtscript_QGraphicsSceneHoverEvent_function_names[_id],
        qtscript_QGraphicsSceneHoverEvent_function_signatures[_id]);
}

QScriptValue qtscript_create_QGraphicsSceneHoverEvent_class(QScriptEngine *engine)
{
    static const int function_lengths[] = {
        1
        // static
        // prototype
        , 0
        , 0
        , 0
        , 0
        , 0
        , 0
        , 0
        , 1
        , 1
        , 1
        , 1
        , 1
        , 1
        , 1
        , 0
    };
    engine->setDefaultPrototype(qMetaTypeId<QGraphicsSceneHoverEvent*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((QGraphicsSceneHoverEvent*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QGraphicsSceneEvent*>()));
    for (int i = 0; i < 15; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QGraphicsSceneHoverEvent_prototype_call, function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_QGraphicsSceneHoverEvent_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<QGraphicsSceneHoverEvent*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_QGraphicsSceneHoverEvent_static_call, proto, function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
