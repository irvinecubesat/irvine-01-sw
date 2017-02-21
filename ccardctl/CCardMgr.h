#ifndef __IRV_CCARD_MGR_HH__
#define __IRV_CCARD_MGR_HH__
namespace IrvCS
{
  /**
   * Manages the C-Card i2c expander access and state.
   **/
  class CCardMgr
  {
  public:
    /**
     * Constructor does all the i2c expander initialization
     **/
    CCardMgr();

    /**
     * Destructor cleans up
     **/
    virtual ~CCardMgr();
    
  private:

    
  };
}
#endif
