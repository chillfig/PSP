<ins>***Describe changes in this MR:***</ins>


The followings are to be verified manually by the developer prior to submitting the MR.
Note that they only need to be done for the affected cFS product(s), and <ins>not all</ins> products like in a CI pipeline.

**MR Checklist**
**For SP0-VxWorks6.9 PSP**
- [ ] Git rebase this product's working Git branch with its origin/development branch
- [ ] Git rebase the ***cert_testbed***'s corresponding Git branch with its origin/development branch
- [ ] Pass cppcheck on <ins>VxWorks</ins> for this product
- [ ] Pass all unit tests on <ins>VxWorks</ins> for this product
- [ ] 100% code coverage for line, function, and branch on <ins>VxWorks</ins> for this product
- [ ] Pass all functional tests on <ins>VxWorks</ins> for this product
