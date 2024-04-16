#pragma once
#include<ESP323248S035.h>

/**
 * This is a utility class for View easyness
 */

class GenericView : public View {
    public:
    protected:
        lv_obj_t *initScreen(lv_obj_t *root){
            if(root==nullptr){
                root=lv_scr_act();
            }
            lv_obj_clean(root);
            lv_obj_clear_state(root, LV_STATE_ANY);
            lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(root, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_pad_left(root, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(root, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(root, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(root, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
            return root;
        }
        lv_obj_t * createLabel(lv_obj_t * parent, char *labeltext, bool dropptr=false){
            lv_obj_t *ret=lv_label_create(parent);
            setLabel(ret, labeltext, dropptr);
            return ret;
        }
        /**
         * This sets the label text and destroys the text when is no longer needed
         * Take this in consideration when calling this method
         */
        void setLabel(lv_obj_t *label, char* txt, bool dropptr=false){
            if(xSemaphoreTake(mx_label,100)==pdTRUE){
                lv_label_set_text(label,txt);
                xSemaphoreGive(mx_label);
#ifdef DEBUG
                Serial.printf("label: %s\n",txt);
            } else {
                Serial.printf("Failed to set label %s\n",txt);
#endif
            }
            if(dropptr)
                delete txt;
        }
        void setObjAttr(lv_obj_t *dest, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y, lv_align_t a){
            lv_obj_set_height(dest, h);
            lv_obj_set_width(dest, w);
            lv_obj_set_x(dest, x);
            lv_obj_set_y(dest, y);
            lv_obj_set_align(dest, a);
        }
        /**
         * Trimming buff
        */
        char * trim(){
            int start=0;
            int end=strlen(buff);
            for(int i=0;i<strlen(buff);i++){
                if(buff[i]!=' '){
                    start=i;
                    break;
                }
            }
            for(int i=strlen(buff)-1;i>start;i--){
                if(buff[i]!=' '){
                    end=i+1;
                    break;
                }
            }
            char *out=new char[end-start+1];
            memset(out,0x0,end-start+1);
            int c=0;
            for(int i=start;i<end;i++)
                out[c++]=buff[i];
            out[c]='\0';
            return out;
        }
        char *buff=new char[256];
        const int8_t SZ_LBL_HEIGHT=18;
        const int8_t SZ_CTR_HEIGHT=40;
        SemaphoreHandle_t mx_label=xSemaphoreCreateMutex();
};

/*
#include"scrn_srch.h"
#include"wifi.h"
#include"scrn_chosewifi.h"
Search::Search(){
}

static void btn_search_cb_(lv_event_t *e){
    Search *self=(Search*)lv_event_get_user_data(e);
    self->btn_search_cb(e);
}

static void txt_search_cb_(lv_event_t *e){
    Search *self=(Search*)lv_event_get_user_data(e);
    self->txt_search_cb(e);
}

void Search::btn_search_cb(lv_event_t *e){
    lv_event_code_t code=lv_event_get_code(e);
    const char *sstr=lv_textarea_get_text(this->txt_search);
    if(strlen(sstr)==0){
        setLabel(this->lbl_status, "No especificó nada para buscar");
    }
    memset(this->buff,0x0,128);
    sprintf(this->buff,"Buscando %s...",sstr);
    setLabel(this->lbl_status,this->buff);
    Serial.println("Se supone que aquí sale a buscar a internet");
}

void Search::txt_search_cb(lv_event_t *e){
    lv_event_code_t code=lv_event_get_code(e);
    if(code==LV_EVENT_FOCUSED){
        lv_keyboard_set_textarea(this->kbd, this->txt_search);
        lv_obj_set_style_max_width(this->kbd,LV_HOR_RES,0);
        lv_obj_update_layout(lv_scr_act());
        lv_obj_set_height(lv_scr_act(), LV_VER_RES- lv_obj_get_height(this->kbd));
        lv_obj_clear_flag(this->kbd, LV_OBJ_FLAG_HIDDEN);
        lv_obj_scroll_to_view_recursive(this->txt_search, LV_ANIM_OFF);
    }
    if(code==LV_EVENT_DEFOCUSED){
        lv_keyboard_set_textarea(this->kbd,NULL);
        lv_obj_set_height(lv_scr_act(),LV_VER_RES);
        lv_obj_add_flag(this->kbd, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL,this->txt_search);
    }
    if(code==LV_EVENT_READY || code==LV_EVENT_CANCEL){
        lv_obj_set_height(lv_scr_act(),LV_VER_RES);
        lv_obj_add_flag(this->kbd,LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(this->txt_search,LV_STATE_FOCUSED);
        lv_indev_reset(NULL,this->txt_search);
    }
}

bool Search::init(lv_obj_t *root){
    if(!Wifi::getInstance().connect()){
        WifiChoser::getInstance().init(root);
        return false;
    }
    root=initScreen(root);

    this->lbl_status=lv_label_create(root);
    this->setObjAttr(this->lbl_status, lv_pct(99), SZ_LBL_HEIGHT, 0, -148, LV_ALIGN_CENTER);
    lv_label_set_long_mode(this->lbl_status, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(this->lbl_status,"Escriba su opcion de busqueda. Letras especiales aun no estan incorporadas.");

    this->txt_search=lv_textarea_create(root);
    this->setObjAttr(this->txt_search, 420, SZ_CTR_HEIGHT, -28, -115, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(this->txt_search, "Buscar...");
    lv_obj_clear_flag(this->txt_search, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);

    this->btn_search=lv_btn_create(root);
    this->setObjAttr(this->btn_search, 50, SZ_CTR_HEIGHT, 213, -115, LV_ALIGN_CENTER);
    lv_obj_add_flag  (this->btn_search, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(this->btn_search, LV_OBJ_FLAG_SCROLLABLE);
    createLabel(this->btn_search, (char*)LV_SYMBOL_RIGHT, false); // TODO: Buscar una lupa o algo
    lv_obj_add_event_cb(this->btn_search, btn_search_cb_, LV_EVENT_ALL, this);

    this->pnl_result = lv_obj_create(root);
    lv_obj_remove_style_all(this->pnl_result);
    this->setObjAttr(this->btn_search, lv_pct(99), 240, 0, 37, LV_ALIGN_CENTER);

    this->kbd=lv_keyboard_create(root);
    lv_obj_set_width (this->kbd, 480);
    lv_obj_set_height(this->kbd, 170);
    lv_obj_set_x     (this->kbd, -1);
    lv_obj_set_y     (this->kbd, 0);
    lv_obj_add_event_cb(this->txt_search, txt_search_cb_, LV_EVENT_ALL, this);
    lv_keyboard_set_textarea(this->kbd,this->txt_search);
    lv_obj_add_flag(this->kbd,LV_OBJ_FLAG_HIDDEN);

    Wifi::getInstance().update();

    return true;
}

void Search::update(msecu32_t now){}

inline std::string Search::title(){ return "Search"; }
*/