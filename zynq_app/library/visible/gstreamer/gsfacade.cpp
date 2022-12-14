#include "gsfacade.h"

#include "library/visible/gstreamer/pipelines/rawrtppipeline.h"
#include "library/visible/gstreamer/pipelines/rawfilepipeline.h"
#include "library/visible/gstreamer/pipelines/rawdisplaypipeline.h"
#include "library/visible/gstreamer/pipelines/internalpipeline.h"

#include "global/logcategories.h"

bool GsFacade::m_interpipelineInited = false;

GsFacade::GsFacade()
{
    gst_init(nullptr, nullptr);

    if (DEBUG_LEVEL > 0) {
        gst_debug_set_active(TRUE);
        gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    }

    InternalPipeline::getInstance().start();
    m_interpipelineInited = true;
}

GsFacade::~GsFacade()
{
    gst_deinit();
}

bool GsFacade::initAndStart(PipelineEnum pipelineEnum)
{
    bool errors = false;

    if (pipelineEnum & RAW_SAVE)
    {
        qCInfo(gsLog()) << "Initializing and starting storage of RAW stream";
        if (!rawPipe)
        {
            rawPipe = std::make_unique<RawFilePipeline>();
        }

        rawPipe->start();
        errors |= !rawPipe->isCompleted();

    }

    if (pipelineEnum & RAW_RTP)
    {
        qCInfo(gsLog()) << "Initializing and starting stream of RAW data over RTP";
        if (!rtpPipe)
        {
            rtpPipe =  std::make_unique<RawRtpPipeline>();
        }

        rtpPipe->start();
        errors |= !rtpPipe->isCompleted();
    }

    if (pipelineEnum & RAW_DISPLAY)
    {
        qCInfo(gsLog()) << "Initializing and starting Display port stream";
        if (!displayPipe)
        {
            displayPipe =  std::make_unique<RawDisplayPipeline>();
        }

        displayPipe->start();
        errors |= !displayPipe->isCompleted();
    }

    return !errors;
}

void GsFacade::stop(PipelineEnum pipelineEnum)
{
    if (pipelineEnum & RAW_SAVE && rawPipe)
    {
        rawPipe ->stop();
    }

    if (pipelineEnum & RAW_RTP && rtpPipe)
    {
        rtpPipe ->stop();
    }

    if (pipelineEnum & RAW_DISPLAY && displayPipe)
    {
        displayPipe ->stop();
    }
}

bool GsFacade::setState(PipelineEnum pipeline, bool state)
{
    if (state)
    {
        return initAndStart(pipeline);
    }
    else
    {
        stop(pipeline);
        return true;
    }
}

GstElement* GsFacade::makeElement(const gchar* factoryName, const gchar* name)
{
    GstElement *res = gst_element_factory_make(factoryName, name);
    qCWarning(gsLog()) << "Element" << name << "of type" << factoryName << (res? "created successfully" : "could not be created.");
    return res;
}

