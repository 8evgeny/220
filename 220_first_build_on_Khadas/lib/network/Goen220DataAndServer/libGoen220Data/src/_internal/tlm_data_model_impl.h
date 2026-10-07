#ifndef TLM_DATA_MODEL_IMPL_H
#define TLM_DATA_MODEL_IMPL_H

#include <QAbstractTableModel>
#include "goen220_tlm_data_model.h"

namespace Goen220 {

class TlmData;

class TlmDataModelImpl : public TlmDataModel
{
    Q_OBJECT


public:
    explicit TlmDataModelImpl(bool editable, QObject *parent = nullptr);

    virtual ~TlmDataModelImpl();

    TlmDataModel::Category category(int r) const;

public:

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;


    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    // TelemetryDataModel interface
public:
    void setRaw(const Tlm *tlm) override;
    const Tlm &rawData() const override;
    Tlm &rawData() override;
    QModelIndex index(int row, Col col) const override;
    int row(size_t offset) const override;
    QPointer<QAbstractItemModel> createCategoryFilterModel(Category cat) override;

private:

    TlmData *m_data;
    Qt::ItemFlag m_eflag;

};

} // namespace Goen220

#endif // TLM_DATA_MODEL_IMPL_H
