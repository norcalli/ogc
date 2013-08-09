#ifndef REVERSE_POLISH_IMPL_H_
#define REVERSE_POLISH_IMPL_H_

template<class T>
const T& ReversePolish<T>::GetValue() const {
  if (values_.size() > 1)
    throw NotEnoughOperators();
  if (values_.empty())
    throw WTFHappened();
  return values_.top();
}

template<class T>
void ReversePolish<T>::PushValue(const T& value) {
  values_.push(value);
}

template<class T>
// void ReversePolish<T>::PopValue(T* value) {
// T& ReversePolish<T>::PopValue() {
T ReversePolish<T>::PopValue() {
  if (values_.empty())
    throw NoParam();
  // *value = values_.top();
  // T& temp = values_.top();
  T temp = values_.top();
  values_.pop();
  return temp;
}

template<class T>
void ReversePolish<T>::HandleOperator(Operator* operation) {
// void ReversePolish<T>::HandleOperator(const Operator& operation) {
  const Operator& op = *operation;
  // printf("CallOperator: name: %s\n", op.display_name);
  dprintf("CallOperator: name: %s\n", op.name);
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
  if (op.function.as<void>() == NULL)
    return;
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
  if (op.argument_count == 0) {
    op.function.as<void()>()();
    return;
  }
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
  if (op.argument_count == 1) {
    // typename Operations<T>::Unary unary = op.function;
    typename Operations<T>::Unary* unary = op.function;
    // T value;
    // PopValue(&value);
    // return unary(value);
    PushValue(unary(PopValue()));
    return;
  }
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
  if (op.argument_count == 2) {
    // typename Operations<T>::Binary binary = op.function;
    typename Operations<T>::Binary* binary = op.function;
    // T& right = PopValue(), &left = PopValue();
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
    T right = PopValue(), left = PopValue();
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
//  std::cout << binary << std::endl;
//  std::cout << left << std::endl;
//  std::cout << right << std::endl;
    PushValue(binary(left, right));
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
    return;
  }
//  std::cout << __FILE__ ":" << __LINE__ << ":DICKS" << std::endl;
  // typename Operations<T>::Nary nary = op.function;
  typename Operations<T>::Nary* nary = op.function;
  // TODO: Todo, should this be T*?
  std::vector<T> arguments(op.argument_count);
  for (long i = arguments.size() - 1; i >= 0; --i) {
    arguments[i] = PopValue();
  }
  PushValue(nary(arguments));
}

template<class T>
 void DefaultRPN<T>::HandleValue(const base_type& value) {
//void ReversePolish<T>::HandleValue(const base_type& value) {
  this->PushValue(T(value));
}

template<class T>
//void ReversePolish<T>::HandleConstant(const char*, const base_type& value) {
 // void DefaultRPN<T>::HandleConstant(const char*, const base_type& value) {
 void DefaultRPN<T>::HandleConstant(const std::string&, const base_type& value) {
  this->PushValue(T(value));
}

// template<class T>
// T DefaultRPN<T>::BinaryOperator(int operation,
//                          const T& left,
//                          const T& right) {
//   if (operation == '-') return left - right;
//   if (operation == '+') return left + right;
//   if (operation == '*') return left * right;
//   if (operation == '/') {
//     if (right == 0)
//       // TODO: Make update.
//       throw "Dividing by zero.";
//     return left / right;
//   }
//   if (operation == '^') return pow(left, right);
//   throw "Unsupported binary operator.";
// }

// template<class T>
// T DefaultRPN<T>::UnaryOperator(int operation, const T& value) {
//   // TODO: Update
//   if (operation == kNeg)   return -value;
//   if (operation == kSin)   return sin(value);
//   if (operation == kCos)   return cos(value);
//   if (operation == kTan)   return tan(value);
//   if (operation == kAsin)  return asin(value);
//   if (operation == kAcos)  return acos(value);
//   if (operation == kAtan)  return atan(value);
//   if (operation == kExp)   return exp(value);
//   if (operation == kLog)   return log(value);
//   if (operation == kLog10) return log10(value);
//   return value;
// }

// template<class T>
// T DefaultRPN<T>::FunctionOperator(int operation) {
//   // T temp;
//   // PopValue(&temp);
//   return UnaryOperator(operation, this->PopValue());//temp);
// }


#endif  // REVERSE_POLISH_IMPL_H_
