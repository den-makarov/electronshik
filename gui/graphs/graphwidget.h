#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include <QWidget>

#include "graphprocessor.h"
#include "plot.h"
#include "zoomselectionrectarea.h"

class QPainter;
class QPaintEvent;

namespace PowerLab {
namespace Gui {

class ZoomSelectionRectArea;

class GraphWidget : public QWidget {
Q_OBJECT
public:
  constexpr static double ZOOM_DEFAULT_FACTOR = 2.0;

  struct GraphData {
    std::string name;
    std::string units;
    std::vector<double> points;
    double minValue = 0.0;
    double maxValue = 0.0;
    QColor color = Qt::black;
    int width = 1;
  };

  GraphWidget(QWidget *parent = nullptr, GraphProcessor* graph = nullptr, Plot* plot = nullptr);
  ~GraphWidget() override;

  void addGraphData(std::string name,
                    std::string units,
                    std::vector<double>&& dataPoints);
  void addHorizontalScaleData(std::string name,
                              std::string units,
                              std::vector<double>&& dataPoints);

  void resetDefaultView();
  void zoomGraph(double factor);

  size_t getGraphCount() const;
  const GraphData* getGraphData(size_t idx) const;
  GraphData* getGraphData(size_t idx);
  const GraphData& getGraphReferenceData() const;
  GraphData& getGraphReferenceData();

  const Plot& getPlot() const;
  Plot& getPlot();

  const GraphProcessor& getGraphProcessor() const;
  GraphProcessor& getGraphProcessor();

  virtual QSize sizeHint() const override;
public slots:
  void plot();
  void setZoomEnabled(bool enable);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  void updateHorizontalScale(double min, double max, double reserveAreaFactor);
  void updateVerticalScale(double min, double max, double reserveAreaFactor);
  void estimateVerticalMinMaxValue(double& min, double& max) const;
  void calcMinMaxGraphValues(GraphData& graph);
  void updateHorizontalLabels();
  void updateVerticalLabels();
  void resetGraphVerticalScale();
  void resetGraphHorizontalScale();

  void setupDefaultPlotMargins() const;

  void zoomFinish();
  void zoomUpdate(QPoint global, QPoint local);
  void zoomBegin(QPoint global, QPoint local);
  bool checkIfPointInGraphLimits(QPoint point) const;
  QRectF calcValuesBoundFromZoomArea(QRect zoomArea) const;
  QRectF zoomValueBoundsByFactor(double factor) const;

  std::vector<GraphData> m_graphs;
  GraphData m_horizontalScale;
  std::unique_ptr<GraphProcessor> m_graphProcessor;
  std::unique_ptr<Plot> m_plot;
  std::unique_ptr<ZoomSelectionRectArea> m_zoomArea = nullptr;
  bool m_zoomEnabled = false;

  constexpr static double PLOT_VERTICAL_EXTENSION = 0.1;
  constexpr static double PLOT_HORIZONTAL_EXTENSION = 0.0;

  constexpr static int PREFERRED_WIDGIT_WIDTH_HINT = 500;
  constexpr static int PREFERRED_WIDGIT_HEIGHT_HINT = 300;
};

} // namespace Gui
} // namespace PowerLab

#endif // GRAPHWIDGET_H
