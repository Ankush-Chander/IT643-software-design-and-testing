# In component: 'reporting_service'
# My requirements.txt is now empty, as I only need the built-in string utils.

# Now I only import from the specific component I actually use.
from company_string_utils import string_utils

class ReportGenerator:
    def generate_report_title(self, title: str):
        # The dependency is now lightweight and focused.
        formatted_title = string_utils.capitalize(title)
        print(f"Report Title: {formatted_title}")

        # My component is no longer coupled to network or database libraries.
        # Changes in those components won't affect me.
