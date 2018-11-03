#include "MultimediaPlayer.hpp"
#include <QtGui/qimage.h>
/**********************************/
/*include ffmpeg*/

/**********************************/
namespace this_cpp_file {
    
    class FrameImage {
    public:
        int width {0};
        int height{0};
        double pts{0};

    };



}/*namespace this_cpp_file*/

namespace sstd {

    Player::Player() {

        mmm_IsStart = create_object_in_this_class<bool>(false);
        mmm_ISLocalFile = create_object_in_this_class<bool>(true);
        mmm_LocalFileName = create_object_in_this_class<QString>();

    }
    
    void Player::setLocalFile(const QString & arg) {
        assert((*mmm_IsStart)==false);
        (*mmm_ISLocalFile) = true;
        (*mmm_LocalFileName) = arg;
    }

    bool Player::start() {
        assert((*mmm_IsStart) == false);
        (*mmm_IsStart) = true;
        if (*mmm_ISLocalFile) {
           return ppp_start_local();
        } else {
           return ppp_start_net();
        }
    }
    
    bool Player::ppp_start_local() {

    }

    bool Player::ppp_start_net() {

    }

}/*namespace sstd*/













