#include"scrn_main.h"
#include <stdio.h>


struct btn_push_call_s {
    MainScreen * self;
    int idx;
};
typedef btn_push_call_s btn_push_call_t;

void btn_push_cb_(lv_event_t *e){
    btn_push_call_t *param=(btn_push_call_t*)lv_event_get_user_data(e);
    MainScreen* self=param->self;
    self->btn_push_cb(param->idx);
}

MainScreen::MainScreen(){}

bool MainScreen::init(lv_obj_t * root){
    if(root == nullptr){
        root=lv_scr_act();
    }
    root=initScreen(root);
    for(int i=0;i<6;i++){
        this->btn_key[i]=lv_btn_create(root);
        char *tmp=new char[24];
        snprintf(tmp,24,"Boton %i",i);
        createLabel(this->btn_key[i], tmp);
        delete[]tmp;
        setObjAttr(this->btn_key[i], 100, 100, 110*(i>2?i-2:i), (i>2?110:0), LV_ALIGN_TOP_LEFT);
        btn_push_call_t *param=new btn_push_call_t;
        param->self=this;
        param->idx=i;
        lv_obj_add_event_cb(this->btn_key[i], btn_push_cb_, LV_EVENT_CLICKED, param);
    }
    return true;
}

void MainScreen::update(msecu32_t const now){}

inline std::string MainScreen::title(){ return "Main";}

void MainScreen::btn_push_cb(int idx){
    Serial.printf("Apretaron el %d\n",idx);
}

