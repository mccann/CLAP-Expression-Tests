#pragma once 

#include "log-event.hpp"

#include <clap/clap.h>
#include <mutex>


static constexpr const size_t max_audio_buffer_events = 4000;

template <typename T>
size_t  PrepareBufferForAppend( T& buffer, size_t max, size_t append_count) {
            const auto size = buffer.size();
            // trim buffer to max size
            if  ( auto total = size + append_count
                ; total > max
            ) {
                auto total_trim = total - max;
                auto buffer_trim = std::max(size,total_trim);
                buffer.erase( buffer.begin(),buffer.begin()+buffer_trim);
                return total_trim-buffer_trim; // amount to trim from appending amount
            }
            return 0;
        }

struct EventLogger
{
    void    reset() {
                // open 
                sample_time = 0;
                block_counter = 0;
                events_buffer.reserve(max_audio_buffer_events);
                events_buffer.clear();
            }

    auto    prepareBufferForPush(uint32_t new_amount) {
                return (uint32_t)PrepareBufferForAppend(events_buffer,max_audio_buffer_events,new_amount);
            }

    void    processBegin(const clap_process_t *process) {
                sample_time = (process->steady_time > 0)
                            ? uint64_t(process->steady_time)
                            : sample_time ;
                block_size = process->frames_count;
                
            }
    void    processEnd() {
                sample_time += block_size;
                ++block_counter;
            }

    void    pushEvent( const clap_event_header* event ) {
                std::lock_guard lock(buffer_mutex);
                prepareBufferForPush(1);
                events_buffer.push_back(LogEvent::from(event,block_counter,block_size));
            }

    auto    processInEvents(const clap_process_t *process) -> clap_process_status {
                processBegin(process);

                const uint32_t num_events = process->in_events->size(process->in_events);
                uint32_t       ev_index = 0;
                {
                    std::lock_guard lock(buffer_mutex);
                    ev_index += prepareBufferForPush(num_events);
                    // push event to buffer
                    for (;ev_index < num_events;++ev_index) {
                        events_buffer.push_back(
                            LogEvent::from(
                                process->in_events->get(process->in_events, ev_index),
                                block_counter,
                                process->frames_count
                            )
                        );
                    }
                }
                
                processEnd();
                return CLAP_PROCESS_CONTINUE;
            }
    
    void    swapEventsBuffer( LogEvent::Buffer& events ) {
                // ensure incoming is large enough - and empty
                events.reserve(max_audio_buffer_events);
                events.clear();
                {
                    std::lock_guard lock{buffer_mutex};
                    std::swap(this->events_buffer,events);
                }
            }

    uint32_t                block_counter{};
    uint32_t                block_size{};
    uint64_t                sample_time{};
    std::mutex              buffer_mutex{};
    LogEvent::Buffer        events_buffer{};
};
