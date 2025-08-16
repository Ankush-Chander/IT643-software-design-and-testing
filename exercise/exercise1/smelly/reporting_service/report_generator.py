from company_common import mega_utils

class ReportGenerator:
    def generate_report_title(self, title: str):
        formatted_title = mega_utils.capitalize(title)

        print(f"Report Title: {formatted_title}")
