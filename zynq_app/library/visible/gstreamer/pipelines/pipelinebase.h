#ifndef PIPELINEBASE_H
#define PIPELINEBASE_H

#include <gst/gst.h>

#include "global/logcategories.h"

/*!
 * \brief Abstract Pipeline class with basic pipeline attributes
 */
class PipelineBase
{
public:
    void start() const;
    void stop() const;

    ~PipelineBase();

    // Disable copy constructors for all pipelines (pipeline subclasses)
    PipelineBase(const PipelineBase&) = delete;
    PipelineBase& operator =(const PipelineBase&) = delete;

    bool isCompleted();

protected:
    PipelineBase() = default;

    void setDefaultCapsFilter(const gchar *name);
    void setQueue(const gchar *name);
    void setSrcFromInternalPipeline(const gchar *name);

    //! \brief gst_add_many and gst_link_many with null check
    template <typename T, typename ...TElems >
    bool addAndLink(T pipeline, TElems ... elements)
    {
        for (auto&& element : {elements...})
        {
            if(!element)
            {
                qCWarning(gsLog()) << "Some of the pipeline elements weren't initialized";
                return false;
            }
        }

        gst_bin_add_many(GST_BIN(pipeline), elements ..., nullptr);

        if (!gst_element_link_many(elements ..., nullptr))
        {
            qCWarning(gsLog()) << "Elements could not be linked.";
            return false;
        }

        return true;
    }

    void completePipeline(const gchar *name);

    struct
    {
        GstElement *videoSrc = nullptr, *capsFilter = nullptr, *queue = nullptr, *sink = nullptr, *pipeline = nullptr;
        GstCaps *videoCaps = nullptr;
        GstBus *bus = nullptr;
    } m_data;

    static const int WIDTH = 1920;
    static const int HEIGHT = 1080;

    bool m_completed = false;
};

#endif // PIPELINEBASE_H
