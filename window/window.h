#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include <vector>
#include <iostream>
#include "host_driver.h"
#include <thread>
#include "ImageProcessor.h"
#include <mutex>
#include <condition_variable>

class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  ~MainWindow();

protected:
  void on_button_clicked();
  void on_button_clicked_1();
  void on_button_clicked_2();
  bool update_count();

  Gtk::Grid grid_;
  Gtk::Button button_;
  Gtk::Button button_1;
  Gtk::Button button_2;
  Gtk::Label fr_;
  Gtk::Label fr;

  Gtk::ScrolledWindow scrolled_window_;
  Gtk::TextView text_view_;
  Gtk::Image image_;

  Driver driver;
  // dnn inf
  Inference inf;
  ImageProcessor *image_processor;
  bool start_flag = false; // 标志变量，用于指示是否需要启动图像处理线程
  bool dma_flag   = false; // 标志变量，用于指示是否需要启动PCIE接受HDMI数据
  bool inf_flag   = false; // 标志变量，用于指示是否需要启动DNN推理
};

#endif // MAIN_WINDOW_H
