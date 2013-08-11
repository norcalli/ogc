#ifndef REVERSE_POLISH_IMPL_H_
#define REVERSE_POLISH_IMPL_H_

template <class Symbol, class Result>
const Result& ReversePolish<Symbol, Result>::GetValue() const {
  if (values_.size() > 1) throw NotEnoughOperators();
  if (values_.empty()) throw WTFHappened();
  return values_.top();
}

template <class Symbol, class Result>
void ReversePolish<Symbol, Result>::PushValue(const result_type& value) {
  values_.push(value);
}

template <class Symbol, class Result>
Result ReversePolish<Symbol, Result>::PopValue() {
  if (values_.empty()) throw NoParam();
  result_type temp = values_.top();
  values_.pop();
  return temp;
}

template <class Symbol, class Result>
void ReversePolish<Symbol, Result>::HandleOperator(Operator* operation) {
  const Operator& op = *operation;
  dprintf("CallOperator: name: %s\n", op.name);
  if (op.function.as<void>() == NULL) return;
  if (op.argument_count == 0) {
    op.function.as<void()>()();
    return;
  }
  if (op.argument_count == 1) {
    typename Operations<result_type>::Unary unary = op.function;
    // typename Operations<result_type>::Unary* unary = op.function;
    PushValue(unary(PopValue()));
    return;
  }
  if (op.argument_count == 2) {
    typename Operations<result_type>::Binary binary = op.function;
    // typename Operations<result_type>::Binary* binary = op.function;
    result_type right = PopValue(), left = PopValue();
    PushValue(binary(left, right));
    return;
  }
  typename Operations<result_type>::Nary nary = op.function;
  // typename Operations<result_type>::Nary* nary = op.function;
  // TODO: Todo, should this be result_type*?
  std::vector<result_type> arguments(op.argument_count);
  for (long i = arguments.size() - 1; i >= 0; --i) {
    arguments[i] = PopValue();
  }
  PushValue(nary(arguments));
}

template <class Symbol, class Result>
void DefaultRPN<Symbol, Result>::HandleValue(double value) {
  this->PushValue(result_type(value));
}

template <class Symbol, class Result>
void DefaultRPN<Symbol, Result>::HandleConstant(const std::string&,
                                                const symbol_type& value) {
  this->PushValue(result_type(value));
}

#endif  // REVERSE_POLISH_IMPL_H_
