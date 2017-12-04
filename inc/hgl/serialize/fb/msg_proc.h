﻿#ifndef HGL_SERIALIZE_FLAT_BUFFERS_MESSAGE_PROC_INCLUDE
#define HGL_SERIALIZE_FLAT_BUFFERS_MESSAGE_PROC_INCLUDE

#include<hgl/type/DataType.h>
#include<flatbuffers/flatbuffers.h>

namespace hgl
{
    namespace serialize
    {
        namespace fb
        {
            class MessageProc
            {
            public:

                using PARSE_FUNC=const void *(*)(const void *);

                struct MSG_PROC
                {
                    PARSE_FUNC parse;
                    DefEvent(bool,proc,(const void *));
                };

            protected:

                uint msg_start;
                uint msg_end;
                uint msg_count;

                MSG_PROC *msg_proc;

            public:

                MessageProc(const uint start,const uint end)
                {
                    msg_start=start;
                    msg_end=end;
                    msg_count=(end-start+1);

                    msg_proc=new MSG_PROC[msg_count];

                    hgl_zero<MSG_PROC>(msg_proc,msg_count);
                }

                virtual ~MessageProc()
                {
                    delete[] msg_proc;      //delete[] nullptr 不是个错误
                }

                bool Map(const uint cmd,MSG_PROC &mp)
                {
                    if(cmd<msg_start||cmd>msg_end)return(false);

                    if(!mp.parse)return(false);
                    if(!mp.proc)return(false);

                    msg_proc[cmd-msg_start]=mp;
                    return(true);
                }

                bool Proc(const uint cmd,const void *data)
                {
                    if(cmd<msg_start||cmd>msg_end)return(false);

                    const void *pack=msg_proc[cmd-msg_start].parse(data);

                    if(!pack)
                        return(false);

                    return msg_proc[cmd-msg_start].proc(pack);
                }
            };//class MessageProc

            #define FB_MSG_MAP(mp,tmp,ns,cn,msg)    tmp.parse=(fb::MessageProc::PARSE_FUNC)flatbuffers::GetRoot<ns::msg>;   \
                                                    SetEventCall(tmp.proc,this,cn,OnProc##msg);    \
                                                    mp.Map(ns##msg,tmp);
        }//namespace fb
    }//namespace serialize
}//namespace hgl
#endif//HGL_SERIALIZE_FLAT_BUFFERS_MESSAGE_PROC_INCLUDE
