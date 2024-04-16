#pragma once
#include<ESP323248S035.h>
#include"scrn_generic.h"
class MainScreen : public GenericView {
    public:
        static MainScreen& getInstance(){
            static MainScreen instance;
            return instance;
        };

        lv_obj_t *btn_key[6];

        bool init(lv_obj_t *) override;
        void update(msecu32_t const) override;
        inline std::string title() override;

        void btn_push_cb(int);
    private:
        MainScreen();
};
