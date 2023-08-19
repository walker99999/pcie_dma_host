
#include "video_dma.h"
typedef std::chrono::high_resolution_clock Clock;

void print_clk(std::string str, std::chrono::_V2::system_clock::time_point t1, std::chrono::_V2::system_clock::time_point t2){
    std::cout << str << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << "ns" << std::endl;
}


DMA::DMA()
    : pcie_fd(0),
        pix_col(0),
        pix_row(0),
        pix_cnt(0),
        frame_rate(0),
        img_finish(false),
        line_valid_cnt(0),
        img(V_NUM, H_NUM, CV_8UC3)
{
    dma_operator = new dma_oper;
    memset(dma_operator, 0, sizeof(dma_oper));
}

DMA::~DMA()
{
    delete dma_operator;
}

//one frame
void DMA::dma_auto_process(int fd, cv::Mat &dst)
{
    dma_operator->current_len = DW_NUM;
    dma_operator->offset_addr = 0;

    memset(dma_operator->write_buf, 0, DMA_MAX_PACKET_SIZE);

    pcie_fd = fd;
    img_finish = false;

    // auto t1 = Clock::now();
    while (!img_finish)
    {
        dma_aline(dst);
    }
    // auto t2 = Clock::now();
    // frame_rate = 1000000000/(((int) std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()));
}

//one line
void DMA::dma_aline(cv::Mat &dst)
{
//auto t1 = Clock::now();

    pcie_trans();

// auto t2 = Clock::now();
// frame_rate = 1000000000/(((int) std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count())*V_NUM);

    dma_rd(dst);

}

// void DMA::dma_rd(cv::Mat &dst)
// {
   
//     uint16_t pix_prim = (static_cast<uint16_t>(dma_operator->read_buf[1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[0]);
//     bool line_valid = (pix_prim == 0xc33c ) || (pix_prim == 0xa55a );
//     // cout << hex << pix_prim << endl;

//     if( line_valid )
//     {
//         for (int i = 4; i < dma_operator->current_len; i++)
//         {
//             for (int j = 0; j < 4; j += 2)
//             {  
//                 uint16_t pix = (static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j + 1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j]);
               
//                     uint8_t r = (pix >> 11) & 0x1f;
//                     uint8_t g = (pix >> 5) & 0x3f;
//                     uint8_t b = pix & 0x1f;
//                     img.at<cv::Vec3b>(pix_row, pix_col) = cv::Vec3b(r << 3, g << 2, b << 3);
//                     if (pix_col == (H_NUM - 1) && ((pix_row == (V_NUM - 1) ) || (pix_prim == 0xa55a )))
//                     {
//                         dst = img;
//                         img_finish = true;
//                     }
                   
//                     pix_col = pix_col == (H_NUM - 1) ? 0 : (pix_col + 1);
//                     pix_row = (pix_col == (H_NUM - 1) && ((pix_row == (V_NUM - 1)||(pix_prim == 0xa55a)))) ? 0
//                                 : ((pix_col == (H_NUM - 1)) ? (pix_row + 1)
//                                 : pix_row);
           
//             }
//         }

//     }

//     line_valid = false;
           
// }


void DMA::dma_rd(cv::Mat &dst)
{
   
    uint16_t pix_prim = (static_cast<uint16_t>(dma_operator->read_buf[1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[0]);
    bool line_valid = (pix_prim < 0x2d0);

    //cout << hex << pix_prim << endl;

    if (pix_prim == 0 )
    {
        dst = img;
    }
    
    pix_row = static_cast<int>(pix_prim);
    
    int k = 0;
    while (k<1000) k++;
    
    if(line_valid )
    {
        //cout << "fuck" << endl;
        for (int i = 4; i < dma_operator->current_len; i++)
        {
            for (int j = 0; j < 4; j += 2)
            {  
                uint16_t pix = (static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j + 1]) << 8) | static_cast<uint16_t>(dma_operator->read_buf[i * 4 + j]);
               
                    uint8_t r = (pix >> 11) & 0x1f;
                    uint8_t g = (pix >> 5) & 0x3f;
                    uint8_t b = pix & 0x1f;
                    img.at<cv::Vec3b>(pix_row, pix_col) = cv::Vec3b(r << 3, g << 2, b << 3);
                    if (pix_col == (H_NUM - 1) && ((pix_row == (V_NUM - 1))))
                    {
                        dst = img;
                        img_finish = true;
                    }
                   
                    pix_col = pix_col == (H_NUM - 1) ? 0 : (pix_col + 1);
           
            }
        }

    }

    line_valid = false;
           
}


void DMA::resume(bool rdy)
{
    if (rdy)
    {
        memset(dma_operator->write_buf, 0x00, DMA_MAX_PACKET_SIZE);

        dma_operator->current_len = DW_NUM;
        dma_operator->offset_addr = 0;

        cout << "pcie state has been reset"  << endl;    

        pcie_trans();
    }
}

/*


*/
void DMA::pcie_trans()
{

    int i=0;

    ioctl(pcie_fd, PCI_MAP_ADDR_CMD, dma_operator);        /* 地址映射,以及数据缓存申请 */
    ioctl(pcie_fd, PCI_WRITE_TO_KERNEL_CMD, dma_operator); /* 将数据写入内核缓存 */

    ioctl(pcie_fd, PCI_DMA_READ_CMD, dma_operator);        /* 将数据写入设备（DMA读） */
    while (i<44000){i++;}
    i=0;

    ioctl(pcie_fd, PCI_DMA_WRITE_CMD, dma_operator); /* 将数据从设备读出到内核（DMA写） */
    while (i<59000){i++;}
    i=0;
    
    
    ioctl(pcie_fd, PCI_READ_FROM_KERNEL_CMD, dma_operator); /* 将数据从内核读出 */
    while (i<5000){i++;}
    ioctl(pcie_fd, PCI_UMAP_ADDR_CMD, dma_operator);        /* 释放数据缓存 */

}

