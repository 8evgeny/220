#include "tlm_data_category_filter_model.h"

namespace Goen220
{

    TlmDataCategoryFilterModel::TlmDataCategoryFilterModel(TlmDataModel::Category cat,
                                                           TlmDataModelImpl *parent)
        : QSortFilterProxyModel(parent), m_cat{cat}
    {
        setSourceModel(parent);
    } /* TlmDataCategoryFilterModel::TlmDataCategoryFilterModel */

    TlmDataModelImpl *TlmDataCategoryFilterModel::tmSrcModel() const
    {
        return qobject_cast<TlmDataModelImpl *>(sourceModel());
    } /* TlmDataCategoryFilterModel::tmSrcModel */

    bool TlmDataCategoryFilterModel::filterAcceptsRow(int source_row,
                                                      const QModelIndex &source_parent) const
    {
        Q_UNUSED(source_parent)

        return (tmSrcModel() != nullptr) ? (tmSrcModel()->category(source_row) == m_cat) : false;
    } /* TlmDataCategoryFilterModel::filterAcceptsRow */

    bool TlmDataCategoryFilterModel::filterAcceptsColumn(int source_column,
                                                         const QModelIndex &source_parent) const
    {
        Q_UNUSED(source_column)
        Q_UNUSED(source_parent)

        return true;
    } /* TlmDataCategoryFilterModel::filterAcceptsColumn */

} // namespace Goen220
