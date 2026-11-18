#pragma once

#include <QSortFilterProxyModel>
#include "tlm_data_model_impl.h"

namespace Goen220 {

class TlmDataCategoryFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TlmDataCategoryFilterModel(TlmDataModel::Category cat, TlmDataModelImpl *parent = nullptr);


protected:

    TlmDataModelImpl *tmSrcModel() const;

private:

    TlmDataModel::Category m_cat = TlmDataModel::Category::UNDEF;

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
};

} // namespace Goen220
