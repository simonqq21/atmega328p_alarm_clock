#include "alarm.h"

extern alarm_settings_t alarm_settings;
extern alarm_memory_t alarm_memory;
extern uint8_t read_rtc;
extern button_action_t prev_adjust_btn_callback, prev_plus_btn_callback, prev_minus_btn_callback;
extern uint8_t prev_btn_callbacks_saved;
extern Button minus_button, adjust_button, plus_button;
extern struct tm *clock_time;

/*
alarm functions
*/

void trim_alarm_values(void)
{
    if (alarm_settings.alarm_hour > 23)
    {
        alarm_settings.alarm_hour = 0;
    }
    if (alarm_settings.alarm_min > 59)
    {
        alarm_settings.alarm_min = 0;
    }
}

void compute_alarm_time(void)
{
    alarm_memory.start_hour = alarm_settings.alarm_hour;
    alarm_memory.start_min = alarm_settings.alarm_min;
    alarm_memory.start_mins_since_midnight = alarm_memory.start_hour * 60 + alarm_memory.start_min;
    alarm_memory.duration_mins = ALARM_DURATION_MINS;
    alarm_memory.end_mins_since_midnight = alarm_memory.start_mins_since_midnight + alarm_memory.duration_mins;

    alarm_memory.end_hour = alarm_memory.end_mins_since_midnight / 60;
    if (alarm_memory.end_hour > 23)
    {
        alarm_memory.end_hour = 0;
    }
    alarm_memory.end_min = alarm_memory.end_mins_since_midnight % 60;
}

/**
 * @brief dismiss alarm button callback
 *
 * dismiss alarm and restore the previous adjust button callback
 */
void dismiss_alarm(void)
{
    alarm_memory.dismissed = 1;

    restore_button_callbacks();
}

void load_alarm_settings(void)
{
    eeprom_read_block((void *)&alarm_settings, (const void *)EEPROM_ALARM_SETTINGS_ADDR, sizeof(alarm_settings_t));
    trim_alarm_values();
    compute_alarm_time();
}

void update_alarm_settings(void)
{
    trim_alarm_values();
    eeprom_update_block((const void *)&alarm_settings, (void *)EEPROM_ALARM_SETTINGS_ADDR, sizeof(alarm_settings_t));
    compute_alarm_time();
}

/**
 *
 */
void set_alarm_dismiss_button_callbacks()
{
    prev_adjust_btn_callback = button_get_single_click_action(&adjust_button);
    prev_plus_btn_callback = button_get_single_click_action(&plus_button);
    prev_minus_btn_callback = button_get_single_click_action(&minus_button);
    prev_btn_callbacks_saved = 1;
    button_attach_single_click(&adjust_button, dismiss_alarm);
    button_attach_single_click(&plus_button, dismiss_alarm);
    button_attach_single_click(&minus_button, dismiss_alarm);
}

/**
 * @brief restore all button callbacks
 */
void restore_button_callbacks(void)
{
    if (prev_btn_callbacks_saved)
    {
        prev_btn_callbacks_saved = 0;
        button_attach_single_click(&adjust_button, prev_adjust_btn_callback);
        button_attach_single_click(&plus_button, prev_plus_btn_callback);
        button_attach_single_click(&minus_button, prev_minus_btn_callback);
    }
}

/**
 *
 */
void alarm_loop(void)
{
    //   == 1
    if (alarm_settings.enabled && alarm_memory.adjusting_time == 0)
    {
        // check if the time is within the alarm time
        alarm_memory.cur_mins_since_midnight = clock_time->hour * 60 + clock_time->min;
        // if the alarm start and end times are within the same day
        bool same_day_alarm_triggered = (alarm_memory.start_mins_since_midnight < alarm_memory.end_mins_since_midnight) &&
                                        (alarm_memory.cur_mins_since_midnight >= alarm_memory.start_mins_since_midnight &&
                                         alarm_memory.cur_mins_since_midnight < alarm_memory.end_mins_since_midnight);
        // if the alarm start and end times are on different days.
        bool different_day_alarm_triggered = (alarm_memory.start_mins_since_midnight > alarm_memory.end_mins_since_midnight) &&
                                             (alarm_memory.cur_mins_since_midnight >= alarm_memory.start_mins_since_midnight ||
                                              alarm_memory.cur_mins_since_midnight < alarm_memory.end_mins_since_midnight);

        if (same_day_alarm_triggered || different_day_alarm_triggered)
        {
            alarm_memory.within_alarm_period = 1;
            if (alarm_memory.prev_triggered == 0)
            {
                alarm_memory.prev_triggered = 1;
                alarm_memory.dismissed = 0;
                // set button callbacks
                set_alarm_dismiss_button_callbacks();
            }
        }
        else
        {
            if (alarm_memory.prev_triggered == 1)
            {
                alarm_memory.prev_triggered = 0;
                restore_button_callbacks();
            }
            alarm_memory.within_alarm_period = 0;
        }

        // check if the time is within the alarm time and n minutes after the alarm time
        /*
        21:00 - 21:05
        21:58 - 22:03
        23:58 - 00:03

        convert hour min times into minutes since midnight
        total_mins = hours * 60 + mins
        total_mins += timedelta
        end_hours = timedelta / 60 % 24;
        end_minutes = timedelta % 60;

        */

        /*
        if alarm is outside alarm period or has been dismissed, stop playing the alarm.
        else if the time is within the alarm time period, and if the alarm has not been manually dismissed,
        start playing the alarm notes.
        */
        if (!alarm_memory.within_alarm_period || alarm_memory.dismissed)
        {
            piezo_stop_sequence();
        }
        else if (alarm_memory.within_alarm_period && !alarm_memory.dismissed && !piezo_is_playing())
        {
            piezo_play_sequence();
        }
    }
    else
    {
        piezo_stop_sequence();
    }
}