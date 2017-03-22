extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/store.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Store::Store(Symbol * symbol, Expression * value, bool clone) :
  m_symbol(symbol),
  m_value(value)
{
  if (clone) {
    m_symbol = (Symbol *)symbol->clone();
    m_value = value->clone();
  }
}

Store::~Store() {
  delete m_symbol;
  delete m_value;
}

Expression::Type Store::type() const {
  return Type::Store;
}

const Expression * Store::operand(int i) const {
  if (i == 0) {
    return m_symbol;
  }
  return m_value;
}

int Store::numberOfOperands() const {
  return 2;
}

Expression * Store::clone() const {
  Expression * newOperands[2];
  newOperands[0] = m_symbol;
  newOperands[1] = m_value;
  return this->cloneWithDifferentOperands(newOperands, 2, true);
}

Expression * Store::cloneWithDifferentOperands(Expression ** newOperands, int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  assert(newOperands[0]->type() == Type::Symbol);
  return new Store((Symbol *)newOperands[0], newOperands[1], cloneOperands);
}

ExpressionLayout * Store::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[3];
  childrenLayouts[0] = m_value->createLayout(floatDisplayMode, complexFormat);
  const char stoSymbol[2] = {Ion::Charset::Sto, 0};
  childrenLayouts[1] = new StringLayout(stoSymbol, 1);
  childrenLayouts[2] = m_symbol->createLayout(floatDisplayMode, complexFormat);
  return new HorizontalLayout(childrenLayouts, 3);
}


Expression * Store::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * valueEvaluation = m_value->evaluate(context, angleUnit);
  context.setExpressionForSymbolName(valueEvaluation, m_symbol);
  delete valueEvaluation;
  return context.expressionForSymbol(m_symbol)->clone();
}

float Store::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

}
