#ifndef ZTOOLSET_DOC_H
#define ZTOOLSET_DOC_H
/**
 *  @namespace  zbs
 *  zbs stands for zbase system.
 *  zbs gathers all base system tools, among which
 *  - zbase toolset
 *   + zbs base datatypes
 *   + ZArray  and derived classes
 *   + ZAM   and derived classes
 *
 *  - ZRandomFile - ZMasterFile + ZIndexFile and derived
 *
 *  This namespace name has been used to avoid any possible name collision (this is what namespace is made for).
 *  This means concretely that when using zbasesystem tools, either
 *    - you must prefix any entity used by zbs::
 *    - you must use the instruction using namespace zbs; in an appropriate location of your code.
 * @warning using namespace zbs ; could generate confusing compilation error in some cases.
 *
 */
namespace zbs {

/** @page ZBaseSystemPage ZBase System Toolset
 * @copydoc ZToolSet
 *
 */


/** @defgroup ZToolSet ZBase System Toolset
 *
 *  A specific namespace zbs has been defined.
 *
 *  zbs stands for zbase system.
 *  zbs gathers all base system tools, among which
 *  - zbase toolset
 *   + zbs base datatypes
 *   + ZArray       and derived classes
 *   + ZArrayMain   and derived classes
 *
 *  - ZRandomFile - ZMasterFile + ZIndexFile and derived classes
 *
 *  This namespace name has been used to avoid any possible name collision (this is what namespace is made for).
 *  This means concretely that when using zbasesystem tools, either
 *    - you must prefix any entity used by zbs::
 *    - you must use the instruction using namespace zbs; in an appropriate location of your code.
 * @warning using namespace zbs ; could generate confusing compilation error in some cases.
 *
 */



/** @addtogroup ZToolSet
 * @defgroup ZBSError ZBase System Error management, Status, Exceptions
 *
 *  Errors / warnings are caracteristed by the following data :
 *
 *  - return status  :
 *      Whenever relevant, a normalized status is returned from any called routine/function/method.
 *  The principle is : any status reporting an error is a negative value.
 *  A positive value indicates either a correct status report or a warning message that has no impact on application flow.
 *  The standard 'correct' status is ZS_SUCCESS.
 *      @see ZStatus
 *
 *  - severity :
 *      Each status is complemented by a severity qualifying the kind of error and its impact on application flow.
 *      @see Severity_type
 *
 *  - exception object :
 *  In case of error/warning signal, an exception object is positionned with appropriate message and data to track error origin.
 * @see CZExceptionMin
 *
 *  @par Throw - no Throw :
 *  Decision has been taken NOT TO THROW any exception.
 *  Throwing exception may cause problems, depending on the platform, when exception is thrown from a different object file than the one form the calling routine.
 *
 *  So that, returned status value has to be tested after each call to zBaseSystem call.
 *
  */

} //zsb

#endif // ZTOOLSET_DOC_H

