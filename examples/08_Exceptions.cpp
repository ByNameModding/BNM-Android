#include <BNM/Exceptions.hpp>
#include <BNM/Method.hpp>

// For example game has "danger" method, that can throw C# exception
// And we want to catch it, to continue code execution even if method thrown exception
BNM::Method<int> DangerMethod;

void (*old_Start)(void *);
void Start(void *instance) {
    old_Start(instance);

    int result;

    // To catch you can use defines
    BNM_try
        result = DangerMethod[instance]();
    BNM_catch(exception /*exception object name*/) // You can skip catch block
        auto className = exception.ClassName();
        auto message = exception.Message();
        BNM_LOG_WARN("DangerMethod returned exception (in try catch) [%s]: %s", className.c_str(), message.c_str());
        result = -1;
    BNM_end_try

    // Or you can use BNM::TryInvoke:
    auto exception = BNM::TryInvoke(/*You can pass anything without args and returning void (lambdas, methods) */ [&]{
        result = DangerMethod[instance]();
    });

    if (exception.IsValid()) {
        auto className = exception.ClassName();
        auto message = exception.Message();
        BNM_LOG_WARN("DangerMethod returned exception (in TryInvoke) [%s]: %s", className.c_str(), message.c_str());
        result = -1;
    }

    if (result == -1) {/*Do sth*/}
}

void OnLoaded_Example_08() {
    auto cls = BNM::Class(BNM_OBFUSCATE("Example"), BNM_OBFUSCATE("Example"));
    DangerMethod = cls.GetMethod(BNM_OBFUSCATE("DangerMethod"));

    BNM::InvokeHook(cls.GetMethod(BNM_OBFUSCATE("Start")), Start, old_Start);
}