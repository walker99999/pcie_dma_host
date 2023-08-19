#include "window.h"

MainWindow::MainWindow()
    : driver(),
    inf()
{
  // const std::vector<std::string> titles = {"test num", "Start size", "End size", "Packet Step", "other1", "other2"};
  // const std::vector<std::string> default_values = {"2025", "2048", "2048", "0", "0", "0"};
  // 创建网格容器
  add(grid_);

  // 设置列和行之间的间距
  grid_.set_column_spacing(10);
  grid_.set_row_spacing(10);

  grid_.attach(image_, 5, 0, 128, 72);

  // 创建按钮
  button_.set_label("Get Image");
  grid_.attach(button_, 1, 0, 1, 1);

  // 创建按钮
  button_1.set_label("INFERENCE OPEN");
  grid_.attach(button_1, 1, 1, 1, 1);

  // 创建按钮
  button_2.set_label("PCIe Resume");
  grid_.attach(button_2, 1, 2, 1, 1);

  // 显示帧率
  fr_.set_text("Frame Rate:");
  grid_.attach(fr_, 1, 10, 1, 1);

  char rate[10];
  sprintf(rate,"%d",0);
  fr.set_text(rate);
  Glib::signal_timeout().connect(sigc::mem_fun(*this,&MainWindow::update_count),1000);//定时器链接触发函数
  grid_.attach(fr, 2, 10, 1, 1);

  // 将输入框数组添加到按钮回调函数中
  button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked));
  button_1.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_1));
  button_2.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_2));

  // 创建图像处理器对象
  image_processor = new ImageProcessor();


  // 创建图像接收线程，在该线程中接收摄像头的图像数据
  std::thread data_trans_thread([&]()
  {
      // cv::Mat dst = cv::imread("../source/data/zidane.jpg");
      // cv::cvtColor(dst, dst, cv::COLOR_BGR2RGB);
      cv::Mat dst;
      while (true)
      {
    
      driver.dma.resume(dma_flag);

      driver.dma.dma_auto_process( driver.getfd(), dst);

      image_processor->store_frame(dst); 

      //count++;

      // char rate[10]; 
      // sprintf(rate, "%d", driver.dma.frame_rate); // 这不是最合适的int to string 方法
      // fr.set_text(rate);


      } });


  // 创建图像显示线程，在该线程中从缓冲区中获取图像数据，并将其显示在Gtk::Image控件中
  std::thread process_display_thread([&]()
   {
      bool start_processing = false; // 标志变量，用于指示是否需要启动图像处理线程
      cv::Mat frame;
      while (true) {
          // 如果标志变量为true，则启动图像处理线程
          if (start_processing) {
          int j=0;
          while (j<1000)
          {
            j++;
          }
          
          //std::cout << "Start processing and displaying !!!" << std::endl;
          frame = image_processor->get_frame();
          if (inf_flag)
            frame = inf.base_exam(frame);
          //cv::resize(frame, frame, cv::Size(1280, 720));
          auto size = frame.size();
          auto pixbuf = Gdk::Pixbuf::create_from_data(frame.data, Gdk::COLORSPACE_RGB, frame.channels() == 4, 8, size.width, size.height, (int)frame.step);
          gdk_threads_enter();
          image_.set(pixbuf);
          gdk_threads_leave();
          // 将标志变量重置为false
          start_processing = false;
          }

          // 检查是否需要启动图像处理线程
          if (start_flag) {
              start_processing = true;
              // start_flag = false;
          }
      } });

    // 显示所有子控件
    show_all_children();
    // 在新线程中执行图像接收和图像显示操作时，主线程可以继续响应其他控件的事件
    data_trans_thread.detach();
    process_display_thread.detach();
}

MainWindow::~MainWindow()
{

}

bool MainWindow::update_count()
{
  /*char rate[10];
  sprintf(rate,"%d",image_processor->count);
  fr.set_text(rate);
  image_processor->count=0;*/
  if(start_flag){
  char rate[10];
  sprintf(rate,"%d",image_processor->count);
  fr.set_text(rate);
  image_processor->count=0;
  }
  else
  {
  char rate[10];
  sprintf(rate,"%d",0);
  fr.set_text(rate);
  image_processor->count=0;
  }
  return true;
}

void MainWindow::on_button_clicked()
{
  start_flag = !start_flag;
  if (start_flag)
    button_.set_label("Stop Image");
  else
    button_.set_label("Get Image");
}

void MainWindow::on_button_clicked_1()
{
  inf_flag = !inf_flag;
  if (inf_flag)
    button_1.set_label("INFERENCE CLOSE");
  else
    button_1.set_label("INFERENCE OPEN");
}

void MainWindow::on_button_clicked_2()
{
  dma_flag = !dma_flag;
  std::cout << "Resume button clicked!!!  resume:"<< dma_flag << std::endl;
}