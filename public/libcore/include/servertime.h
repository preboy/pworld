#pragma once

void    time_init();
void    time_release();

int64   get_frame_time();
int64   get_current_time();
void    update_frame_time();

uint64  get_frame_tick();
uint64  get_current_tick();
void    update_frame_tick();
