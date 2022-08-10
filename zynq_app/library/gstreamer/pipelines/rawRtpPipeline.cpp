#include "rawRtpPipeline.h"

#include "global/config.h"
#include "global/logCategories.h"
#include "library/gstreamer/gsWrapper.h"

RawRtpPipeline::RawRtpPipeline()
{
    setSrcFromInternalPipeline("rtpsource");
    setDefaultCapsFilter("rtpcaps");
    setRtpPayload();
    setUdpsink();
    completePipeline();
}

void RawRtpPipeline::setRtpPayload()
{
    m_rtpvrawpay = GsWrapper::makeElement("rtpvrawpay", "rtpvrawpay");
}

void RawRtpPipeline::setUdpsink()
{
    m_data.sink = GsWrapper::makeElement("udpsink", "udpsink");

    g_object_set(m_data.sink, "port", glb::rtp::PORT, nullptr);
    g_object_set(m_data.sink, "host", glb::rtp::IP_ADDRESS.c_str(), nullptr);
}

void RawRtpPipeline::completePipeline()
{
    m_data.pipeline = gst_pipeline_new("rtppipeline");
    m_data.bus = gst_element_get_bus(m_data.pipeline);
    gst_bus_add_signal_watch(m_data.bus);

    gst_bin_add_many(GST_BIN(m_data.pipeline), m_data.videoSrc, m_data.capsFilter, m_rtpvrawpay, m_data.sink, nullptr);

    bool result = gst_element_link_many(m_data.videoSrc, m_data.capsFilter, m_rtpvrawpay, m_data.sink, nullptr);
    checkResult(result);
}

RawRtpPipeline::~RawRtpPipeline()
{
    stop();

    if (m_rtpvrawpay)
        gst_object_unref(m_rtpvrawpay);
}
