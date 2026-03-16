// Demonstrates using Head and Tail hooks to log method calls and timing
// without modifying the original class.
//
// A common use case is observability: you want to know when methods are
// called, what arguments they received, and how long they took, without
// touching the original implementation.

#include <chrono>
#include <print>
#include <splice/splice.hpp>
#include <string>

class PaymentProcessor {
public:
  [[= splice::hookable{}]] bool processPayment(std::string_view account,
                                                float            amount)
  {
    // Simulate processing
    return amount > 0.0f;
  }

  [[= splice::hookable{}]] void refund(std::string_view account,
                                       float            amount)
  {
    // Simulate refund
  }
};

SPLICE_REGISTRY(PaymentProcessor, g_payments);

// Tracks when a method started so the Tail hook can compute elapsed time.
thread_local std::chrono::steady_clock::time_point g_call_start;

void setupLogging() {
  // Log every processPayment call with its arguments, then log the result
  // and elapsed time after it returns.
  g_payments->inject<^^PaymentProcessor::processPayment,
                     splice::InjectPoint::Head>(
      [](splice::CallbackInfoReturnable<bool>& ci,
         PaymentProcessor*,
         std::string_view account,
         float            amount) {
          g_call_start = std::chrono::steady_clock::now();
          std::println("[log] processPayment called: account: {}, amount: {:.2f}",
              account, amount);
      });

  g_payments->inject<^^PaymentProcessor::processPayment,
                     splice::InjectPoint::Tail>(
      [](splice::CallbackInfoReturnable<bool>& ci,
         PaymentProcessor*,
         std::string_view,
         float) {
          auto elapsed = std::chrono::steady_clock::now() - g_call_start;
          auto ms = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
          std::println("[log] processPayment returned {} in {}µs",
              ci.return_value.value_or(false), ms);
      });

  // Log refunds: void method so CI has no return value
  g_payments->inject<^^PaymentProcessor::refund,
                     splice::InjectPoint::Head>(
      [](splice::CallbackInfo&,
         PaymentProcessor*,
         std::string_view account,
         float            amount) {
          std::println("[log] refund called: account: {}, amount: {:.2f}",
              account, amount);
      });
}

int main() {
  setupLogging();

  PaymentProcessor processor;

  g_payments->dispatch<^^PaymentProcessor::processPayment>(
      &processor, "acc-001", 49.99f);

  g_payments->dispatch<^^PaymentProcessor::processPayment>(
      &processor, "acc-002", -5.00f);

  g_payments->dispatch<^^PaymentProcessor::refund>(
      &processor, "acc-001", 49.99f);
}