#include <getopt.h>
#include "window.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    gdk_threads_init();
    gdk_threads_enter();

    // 创建主窗口
    MainWindow window;
    window.set_title("CICC2111  果宝特工队");
    window.set_default_size(1280, 720);

    gdk_threads_leave();

    // 显示窗口及其所有子控件
    return app->run(window);
}